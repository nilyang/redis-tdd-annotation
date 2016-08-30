#define IDENT 0
#define OP +
//p353 图5-3 《深入了解计算机系统第二版》

typedef int data_t;
typedef struct {
    long int len;
    data_t *data;
} vec_rec, *vec_ptr;

//create vector of specified length
vec_ptr new_vec(long int len)
{
    //allocate header 
    vec_ptr result = (vec_ptr)malloc(sizeof(vec_rec));
    if(!result)
        return NULL;// fail to allocate
    result->len = len;

    //allocate array
    if (len > 0) {
        data_t *data =(data_t *)calloc(len, sizeof(data_t));
        if (!data) {
            free((void*)result);
            return NULL;//fail
        } 

        result->data = data;//        
    } else {
        result->data = NULL;
    }

    return result;
}

// Retrieve vector element and store at dest
// Return 0 (out of bounds) or 1 (successful)
int get_vec_element(vec_ptr v, long int index, data_t* dest)
{
    if (index < 0 || index >= v->len){
        return 0;
    }

    *dest = v->data[index];
    return 1;
}

long int vec_length(vec_ptr v)
{
    return v->len;
}

void combine1(vec_ptr v, data_t *dest)
{
    long int i;
    long int len;
    *dest = IDENT;
    for (i=0,len=vec_length(v); i<len; i++){
        data_t val;
        get_vec_element(v,i,&val);
        *dest = *dest OP val;
    }
}