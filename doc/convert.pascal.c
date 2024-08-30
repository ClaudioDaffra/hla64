#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h> 
#include <stdint.h> 

int convert_dieeetomsbin(const double src8, double *dest8) 
{
    unsigned char ieee[8];

    unsigned char *msbin = (unsigned char *)dest8;
    unsigned char sign;
    unsigned char any_on = 0;
    short msbin_exp;
    int i;

    // Make a clobberable copy of the source number
    memcpy(ieee, &src8, 8);

    for (i = 0; i < 8; i++)
        msbin[i] = 0;

    // If all are zero in src8, the msbin should be zero
    for (i = 0; i < 8; i++)
        any_on |= ieee[i];
	
    if (any_on == 0) {
        return 0;
    }

    sign = ieee[7] & 0x80;
	
    msbin[6] |= sign;
	
    msbin_exp = (ieee[7] & 0x7f) << 4;
    msbin_exp += (ieee[6] >> 4);

    if ((msbin_exp - 0x3ff) > 0x80) 
	{
        return 1;
    }

    msbin[7] = msbin_exp - 0x3ff + 0x80 + 1;

    // The ieee mantissa must be shifted up 3 bits
    ieee[6] &= 0x0f; // mask out the exponent in the second byte
    for (i = 6; i >= 1; i--) {
        msbin[i] |= (ieee[i] << 3);
        msbin[i] |= (ieee[i - 1] >> 5);
    }
    msbin[0] |= (ieee[0] << 3);

    return 0;
}

char* convertDoubleToMBF( double fs  )
{
	double fd = 0 				;
	
	convert_dieeetomsbin( fs, &fd )  	;

    unsigned char *mbf = (unsigned char *)&fd;

	char f40s[128] ;
	snprintf ( f40s , 128 , "%02x,%02x,%02x,%02x,%02x" , mbf[7],mbf[6],mbf[5],mbf[4],mbf[3] ) ;
	
	return strdup((char*)f40s);
}


int convert_fieeetomsbin(const float src4, float *dest4) 
{
    uint8_t *ieee = (uint8_t *)&src4;
    uint8_t *msbin = (uint8_t *)dest4;
    uint8_t sign;
    uint8_t msbin_exp;
    int i;

    msbin_exp = 0;
    // See _fmsbintoieee() for details of formats
    sign = ieee[3] & 0x80;
    msbin_exp |= (ieee[3] << 1);
    msbin_exp |= (ieee[2] >> 7);
    // An ieee exponent of 0xfe overflows in MBF
    if (msbin_exp == 0xfe) {
        return 1;
    }
    msbin_exp += 2; // actually, -127 + 128 + 1
    for (i = 0; i < 4; i++) {
        msbin[i] = 0;
    }
    msbin[3] = msbin_exp;
    msbin[2] |= sign;
    msbin[2] |= (ieee[2] & 0x7f);
    msbin[1] = ieee[1];
    msbin[0] = ieee[0];
	
    return 0;
}

char* convertFloatToMBF( float fs  )
{
	float fd = 0 				;
	
	convert_fieeetomsbin( fs, &fd )  	;

    unsigned char *mbf = (unsigned char *)&fd;

	char f40s[128] ;
	snprintf ( f40s , 128 , "%02x,%02x,%02x,%02x,%02x" , mbf[7],mbf[6],mbf[5],mbf[4],mbf[3] ) ;
	
	return strdup((char*)f40s);
}


int main ( void ) 
{
    /*
	.byte  $87, $76, $e9, $78, $d4       123.456
	.byte  $96, $8f, $05, $29, $4b  -2343242.3234
	.byte  $91, $86, $b4, $66, $1e    -68968.7978
	.byte  $8a, $76, $dd, $24, $58       987.4553434
	.byte  $91, $9b, $d6, $b0, $a0    -79789.3798979
	.byte  $84, $4c, $c4, $86, $ad        12.79798
	.byte  $7f, $6d, $47, $26, $a2         0.46343346346
	.byte  $88, $ea, $52, $ca, $57      -234.3234
	.byte  $91, $9b, $d6, $e5, $17    -79789.78979
    
    .byte  $81, $19, $99, $99, $99   1.2
    .byte  $82, $d9, $99, $99, $99  -3.4
    .byte  $83, $33, $33, $33, $33   5.6
    .byte  $83, $f9, $99, $99, $99  -7.8
    .byte  $84, $11, $99, $99, $99   9.1
    .byte  $84, $24, $cc, $cc, $cc  10.3
    .byte  $84, $b6, $66, $66, $66 -11.4
    .byte  $84, $48, $00, $00, $00  12.5
    
    .byte  $85, $84, $cc, $cc, $cc -16.6
    
    .byte  $85, $17, $33, $33, $33  18.9
*/

 
	//.byte  $7f, $6d, $47, $26, $a2         
	// 0.46343346346
 
	double fs = 0.46343346346 ;
	
	char* mbs1 = convertDoubleToMBF( fs  ) ;
	
	printf ( "\n[ %g %s ]",fs,mbs1 );
 
    printf("\n");

	float fs2 = 0.46343346346 ;
	
	char* mbs2 = convertDoubleToMBF( fs2  ) ;
	
	printf ( "\n[ %g %s ]",fs2,mbs2 );
 
    printf("\n");
	
    return 0 ;
}