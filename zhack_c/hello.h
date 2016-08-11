#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void test_string();
void test_array();
void test_struct();


void test_string()
{
        typedef char * sds;
        const sds s = "hellow world\n";
        printf("%s\n%p ,len = %zu\n", s,&s,strlen(s));
        free(s);
}

void test_array()
{
        int array[] = {21,33,35};
        printf("\n array address:\n%p\n%p\n%p\n", array, &array, &array[0]);
	
        int *array_ptr= array;
        printf(" 1st element:%p = %d \n", array_ptr , *array_ptr);
        array_ptr++;
	    printf(" 2d element:%p = %d \n",  array_ptr, *array_ptr);
        array_ptr++;
        printf(" 3rd element:%p = %d \n", array_ptr, *array_ptr);

        int *arr_ptr = &array[1];
        printf("\n 1st elem for arr_ptr: %p = %d\n", arr_ptr, arr_ptr[0]);
        printf("\n 2d elem for arr_ptr: %p = %d\n", (arr_ptr+1), arr_ptr[1]);
        //stack overflow
        //printf("\n 3rd elem for arr_ptr: %p = %d\n", (arr_ptr+2), arr_ptr[2]);
        free(array_ptr);
}


void test_struct()
{
        struct foo {
                size_t size; //8 Byte
                char   name[64]; // 64 Byte
                int    anser_to_ultimate_question; //4 Byte
                unsigned shoe_size; //4 Byte
        };

        struct foo my_foo;
        const char *pstr =  "hello world!";
        my_foo.size = sizeof(struct foo);
        memcpy(my_foo.name, pstr, strlen(pstr));
        my_foo.anser_to_ultimate_question = 4;
        my_foo.shoe_size = 23;
        printf("my_foo.size = %zu\n",my_foo.size);
        printf("\nsizeof(size_t) = %zu\nsizeof(char[64]) =%zu\nsizeof(int)=%zu\nsizof(unsigned)=%zu\n",
            sizeof(size_t),
            sizeof(char[64]),
            sizeof(int),
            sizeof(unsigned)
        );

        printf("\n&my_foo.name=%p\n&my_foo=%p, unsigned long=%zu, unsigned=%zu\n",&my_foo.name, &my_foo,sizeof(unsigned long),sizeof(unsigned));
        
}

void test_allocate()
{
    int *p= (int *)malloc(sizeof(int));
    *p = 100;
    printf("%d\n", *p);
    free(p);
    
    // in C++
    // int *p2 = new int;
    // *p = 100;
    // delete p; 
}


void test_arr()
{
    //int a[] = {6,1,2,3,4,5};
    //int a[] = {0};
    int a[] = {[1]=2,4,[5]=6,}; // c99 
    for(int i=0,len=sizeof(a)/sizeof(int);i<len;i++){
        printf("&a[%d]=%p %d\n",i,&a[i], a[i]);
    }
}

int test_arr_search(int key, int a[], int len)
{
   int ret = -1;
   int i;
   for (int i=0; i <len;i++){
      if ( a[i] == key){
         ret = i;
         break;
      }
   }

  return ret;
}

int test_array_check(int key, int a[], int len)
{
   int ret = -1;
   int i;

   for(i=0; i< len; i++){
       
   }
   return ret;
}
