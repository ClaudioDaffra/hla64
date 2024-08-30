
hla64 version alpha 0.0.1

(experimental not for production!)

Project of a high-level assembler for Commodore machines Interfaces with tass64:

    types                   :   char byte int word real (f40)
    stack                   :   possibility to move the stack to zero page
    pointers and addresses  :   like in C, management of pointers and addresses
    structures              :
    implemented operations  :
        unary   : $cast ! - + * &
        binary  : + - * / % :=
    multi-dimensional arrays: implemented through pointer arithmetic
    functions
    automatic casting
    initial optimisation routine-fast

need :

    GCC
    Visual Studio
    64tass Assembler for C64

build :

    Linux   :     make -f lin_gcc_make
    Windows :     nmake /f win_cl_make

usage :

    bin/cd -i tst/utf8.txt -o tst/a.out -g
    64tass -C -a -B -i tst/utf8.txt.asm -o tst/x.prg

example :

    program 

        int intero
        int * gppluto 
        char  pippo 
        
        strutturablobale     {
            byte gx_byte_1
            word gy_word_2
            real gz_real_5
            int aaaa
        }

        function sub () -> char
            real  x
            real *px
            word *pw
        {
            word  pluto1 
         
             pluto1 := pw+1
        }

        function fff () -> int
            word pw
            byte pb
            real pr
        {
            byte  lb
            real  lr
            word  *ptrw
            word  provaW

            gppluto + 1
            
            pw := &ptrw

            provaW := &  1

            asm {
                stack_load_address provaW
                stack_load_word
                jsr std.print_u16_dec
            }
            
        }

        function main () -> int
        {
            point 
            {
                int x
                char y
            }

            rect
            {
                int a
                real b
                char c
            }
           
            word  plutow 
            byte  plutob
            word  *pword
            word  uw16
            byte  *pbyte
            byte  risbyte


            point p2 
            
            rect *rr1
            
            rr1+2
            
            pword := p2.x 
            
            asm {
                nop
                nop
                nop
            }
            
            p2.x := 1235 ;

            plutob := 123
            
            asm { ;49660 
                stack_load_address plutob
                jsr std.print_u16_dec
            }
            asm { ; 123
                stack_load_address plutob
                stack_load_byte
                jsr std.print_u8_dec
            }

            pbyte := &plutob
            
            asm { ; 49655
                stack_load_address pbyte
                jsr std.print_u16_dec
            }
            asm { ;  49660
                stack_load_address pbyte
                stack_load_word
                jsr std.print_u16_dec
            }

            risbyte := *pbyte

            asm { ; 49654
                stack_load_address risbyte
                jsr std.print_u16_dec
            }
            asm { ; 123
                stack_load_address risbyte
                stack_load_byte
                jsr std.print_u8_dec
            }

            uw16 := pbyte + 1

            asm { ; 49656
                stack_load_address uw16
                stack_load_word
                jsr std.print_u16_dec
            }

            pbyte := &plutob
            
            asm { ; 44
                nop 
                nop  
                nop 
            }
            
            *pbyte :=  44

            asm { ;; 44
                stack_load_address plutob
                stack_load_byte
                jsr std.print_u8_dec
            }

            $int  pbyte + 2

            $byte (  1 +    ( ! 1.0 ) )

            asm { 
                nop 
                nop  
                nop 
            }
            
            *(pbyte+2) :=  44

        }

    end



