#include "hashMap.h"

union hmData hmValue ;

/* initialize a hash map structure */
short int hashMapInit
(
    hashMap_t* map, 
    size_t (*hmHashFN)(void* key), 
    int(* hmEqFN)(void* p1, void* p2), 
    size_t start_len, 
    float load_factor
)
{
    map->table = calloc(start_len, sizeof(hmNode_t*));
    if(!map->table)
        return HM_ERR_ALLOC;
    
    map->hmHashFN = hmHashFN;
    
    map->hmEqFN = hmEqFN;
    map->table_len = start_len;
    map->load_factor = load_factor;
    map->element_ct = 0;

    return 0;
}

/* add a key-value pair to a hash map, resizing and rehashing if necessary */
short int hashMapPut(hashMap_t* map, void* key, void* value, unsigned short int flags)
{
	hmNode_t* n_node,
		* s_node;
	size_t node_idx,
		node_hash;
	
	/* perform resize and rehash if necessary */
	if(((float)(++ map->element_ct)) / map->table_len > map->load_factor)
	{
		size_t i;
		
		size_t n_len = map->table_len << 1;
		hmNode_t** temp = calloc(n_len, sizeof(hmNode_t*));
		if(!temp)
		{
			map->element_ct --;
			return HM_ERR_ALLOC;
		}

		/* for each element in the table */
		for(i = 0; i < map->table_len; i ++)
		{
			/* traverse down the linked list */
			hmNode_t* current,
				* next;
			
			/* guard against empty elements */
			current = map->table[i];
			while(current)
			{
				size_t npos;
				
				/* prepare lookahead pointer */
				next = current->next;
				
				/* rehash and copy each item */
				npos = (map->hmHashFN(current->key)) % n_len;
				current->next = temp[npos];
				temp[npos] = current;
				
				/* advance to next list element */
				current = next;
			}
		}
		
		free(map->table);
		map->table = temp;
		map->table_len = n_len;
	}
	
	/* check whether item is already in map */
	node_hash = (*map->hmHashFN)(key);
	node_idx = node_hash % map->table_len;
	s_node = map->table[node_idx];
	while(s_node)
	{
		if(flags & HM_FAST ? ((map->hmHashFN)(s_node->key) == node_hash) : ((map->hmEqFN)(s_node->key, key)))
		{
			map->element_ct --;

			/* deallocate existing value if necessary*/
			if(flags & HM_DESTROY)
				free(s_node->value);
			
			/* update value and return if found */
			s_node->value = value;
			return 0;
		}
		s_node = s_node->next;
	}
	
	/* create a new hmNode_t to hold data */
	n_node = malloc(sizeof(hmNode_t));
	if(!n_node)
	{
		map->element_ct --;
		return HM_ERR_ALLOC;
	}
	n_node->key = key;
	n_node->value = value;
	n_node->next = map->table[node_idx];
	
	/* add new hmNode_t to table */
	map->table[node_idx] = n_node;
	
	return 0;
}

/* get the value associated with a key
 * returns NULL if key does not exist in map
 */
void* hashMapGet(hashMap_t* map, void* key, unsigned short int flags)
{
	hmNode_t* current;
	size_t k_hash;
	
	k_hash = (map->hmHashFN)(key);
	current = map->table[k_hash % map->table_len];
	while(current)
	{
		if(flags & HM_FAST ? (k_hash == (map->hmHashFN)(current->key)) : ((map->hmEqFN(key, current->key))))
			return current->value;
		
		current = current->next;
	}
	
	return NULL;
}

/* removes an item from the hash map if present (else no-op) */
short int hashMapDrop(hashMap_t* map, void* key, unsigned short int flags)
{
	hmNode_t* current,
		* parent = NULL;
	size_t k_hash,
		idx;

	k_hash = (map->hmHashFN)(key);
	idx = k_hash % map->table_len;
	
	current = map->table[idx];
	while(current)
	{
		if(flags & HM_FAST ? ((map->hmHashFN)(current->key) == k_hash) : ((map->hmEqFN)(key, current->key)))
		{
			/* redirect the hmNode_t chain around it, then destroy */
			if(parent)
				parent->next = current->next;
			else
				map->table[idx] = current->next;
			if(flags & HM_DESTROY)
				free(current->value); /* key can be freed from calling code; would be too messy to add that here */
			free(current); /* nodes are always malloc'ed */
			
			/* perform resize and rehash if necessary */
			if((map->table_len > 10) && (-- map->element_ct) / (map->table_len << 1) < map->load_factor)
			{
				size_t i;
				
				size_t n_len = map->table_len >> 1;
				hmNode_t** temp = calloc(n_len, sizeof(hmNode_t*));
				if(!temp)
				{
					map->element_ct --;
					return HM_ERR_ALLOC;
				}
				
				/* for each element in the table */
				for(i = 0; i < map->table_len; i ++)
				{
					/* traverse down the linked list */
					hmNode_t* current,
						* next;
					
					/* guard against empty elements */
					current = map->table[i];
					while(current)
					{
						size_t npos;
						
						/* prepare lookahead pointer */
						next = current->next;
						
						/* rehash and copy each item */
						npos = (map->hmHashFN(current->key)) % n_len;
						current->next = temp[npos];
						temp[npos] = current;
						
						/* advance to next list element */
						current = next;
					}
				}
				
				free(map->table);
				map->table = temp;
				map->table_len = n_len;
			}
			
			/* stop looking since hmNode_t was found */
			return 0;
		}
		
		parent = current;
		current = current->next;
	}
	
	return HM_NOT_FOUND;
}

/* destroys a hash map (does not touch pointed data) */
void hashMapDestroy(hashMap_t* map, unsigned short int flags)
{
    size_t i;
    hmNode_t    * current,
                * next;

    /* goes down each list, deleting all nodes */
    for(i = 0; i < map->table_len; i++)
    {
        current = map->table[i];
        while(current)
        {
            next = current->next;
            if(flags & HM_DESTROY)
            {
                free(current->key);
                free(current->value);
            }
            free(current);
            current = next;
        }
    }

    /* deletes the table */
    free(map->table);
}

/* simple "hash" function (uses pointer as hash code) */
size_t hmPtrHash(void* key)
{
	return (size_t) key;
}

/* simple string hash function */
size_t hmStringHash(void* key)
{
	size_t hash = 0;
	char* string = key;
	
	while(*string)
	{
		hash += *string; /* addition followed by multiplication makes collisions of scrambled strings with the same characters less likely */
		hash *= 7;
		string ++;
	}
	
	return hash;
}

/* simple equality checker (compares pointers) */
int hmPtrEq(void* p1, void* p2)
{
	return p1 == p2;
}

/* string equality checker (uses strcmp) */
int hmStringEq(void* p1, void* p2)
{
	return !strcmp((char*)p1, (char*)p2);
}