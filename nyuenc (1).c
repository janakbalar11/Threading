


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <ctype.h>


char *string; 
size_t string_size;

char* concat(FILE *file) {
    char buffer[BUFSIZ];
    size_t n;
    while ((n = fread(buffer, 1, BUFSIZ, file)) > 0) {
        string = realloc(string, string_size + n);
        memcpy(string + string_size, buffer, n);
        string_size += n;
    }
    return string;
}

void binOutput(char* str, size_t size) {
    char current = str[0];
    int count = 1;

    for (size_t i = 1; i < size; i++) {
        if (str[i] == current) {
            count++;
        } else {
            unsigned char out[2] = {current, count};
            fwrite(out, sizeof(out), 1, stdout);
            current = str[i];
            count = 1;
        }
    }

    unsigned char out[2] = {current, count};
    fwrite(out, sizeof(out), 1, stdout);
}


typedef struct {
    int id;
    char *output;
    int count;
} task_result;

typedef struct taskqueue{
  int id;
  char *data;
 struct taskqueue *next;
} taskqueue;


taskqueue *tasks = NULL;
bool tasks_comp = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t result_cond = PTHREAD_COND_INITIALIZER;
int taskcount;
int completed_tasks = 0;
task_result *task_results;



void *thread_func(void *arg){
  taskqueue *curr;
  bool done = false;
  // task_result *result = (task_result *) malloc(sizeof(task_result));;
  task_result *result;
  
  while(!done){
    pthread_mutex_lock(&mutex);
    while(tasks == NULL && !tasks_comp){
      pthread_cond_wait(&cond, &mutex);
    }
    if(tasks == NULL && tasks_comp){
      done = true;
      // pthread_cond_signal(&result_cond);
      pthread_mutex_unlock(&mutex);
      break;
    }
    curr = tasks;
    tasks = curr->next;
    if(tasks == NULL){
      tasks_comp = true;
    }
    pthread_mutex_unlock(&mutex);

    int data_len = strlen(curr->data);
    int count = 1;

    char *output = (char *) malloc(sizeof(char) * (data_len+1));

    int output_len = 0;
    
    for(int i = 0; i < data_len; i++){
      if(curr->data[i] == curr->data[i+1]){
        count++;
      }
      else{
        output_len += sprintf(output+output_len, "%c%d", curr->data[i], count);
        count = 1;
      }
    }

  free(curr->data);

    result = (task_result *) malloc(sizeof(task_result));
    result->id = curr->id;
    result->output = output;
    
        pthread_mutex_lock(&mutex);

     task_results[curr->id] = *result;
    completed_tasks++;
    pthread_cond_signal(&result_cond);
      free(curr);
      free(result);
     pthread_mutex_unlock(&mutex);

     

  }
  
  return NULL;
}
int main(int argc, char **argv){
    

  int opt;

 int num_threads = 1;

  while((opt = getopt(argc,argv,"j:"))!=-1){
    switch(opt){
      case 'j':
      num_threads = atoi(optarg);
      break;
    
    default:
    exit(1);
  }
    }
if(num_threads>1){

pthread_t threads[num_threads]; 
// int thread_id[num_threads];

for(int i = 0; i< num_threads; i++){
  // thread_id[i] = i;
  if(pthread_create(&threads[i], NULL, thread_func, NULL)){
   fprintf(stderr,"error");
    exit(1);
  }
}
  task_results = (task_result *) malloc(sizeof(task_result) * 250000);
  int task_id = 0;
  taskqueue *curr;

      int total_chunks = 0;


for (int index = optind; index < argc; index++) {
    FILE* f = fopen(argv[index], "r");
    size_t total_bytes_read = 0;
    int chunk_size = 4096;
    char buffer[chunk_size];
    while (!feof(f)) {
        size_t bytes_read = fread(buffer, 1, chunk_size, f);
        if (bytes_read > 0) {
            taskqueue* newNode = malloc(sizeof(taskqueue));
            newNode->id = task_id++;
            newNode->data = malloc(bytes_read + 1);
            memcpy(newNode->data, buffer, bytes_read);
            newNode->data[bytes_read] = '\0';
            newNode->next = NULL;
            pthread_mutex_lock(&mutex);
            if (tasks == NULL) {
                tasks = newNode;
                curr = tasks;
            } else {
                curr->next = newNode;
                curr = curr->next;
            }
            taskcount++;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
            total_chunks++;
            total_bytes_read += bytes_read;
        }
    }
    fclose(f);

}

  


char current_letter = '\0';
int current_num = 0;
int comp_tasks = 0;
int prev_task = -1; 
char output_buf[10000];
int output_idx = 0;
int index = 0;

while (completed_tasks < taskcount) {
  pthread_mutex_lock(&mutex);
    pthread_cond_wait(&result_cond, &mutex);
    pthread_mutex_unlock(&mutex);

    for (int i = index; i < task_id; i++) {
        if (task_results[i].output != NULL && i == prev_task + 1) { 
            char* str = task_results[i].output;
            int len = strlen(str);
            int idx = 0;

            while (idx < len) {
                if (isdigit(str[idx])) {
                    int num = 0;
                    while (idx < len && isdigit(str[idx])) {
                        num = num * 10 + (str[idx] - '0');
                        idx++;
                    }
                    current_num += num;
                } else {
                    char c = str[idx];
                    if (c != current_letter) {
                        if (current_letter != '\0') {
                            output_buf[output_idx++] = current_letter;
                            *(unsigned char*)&output_buf[output_idx] = (unsigned char)current_num;
                            output_idx += sizeof(unsigned char);
                            if (output_idx >= sizeof(output_buf)) {
                                pthread_mutex_lock(&result_mutex);
                                fwrite(output_buf, 1, output_idx, stdout);
                                pthread_mutex_unlock(&result_mutex);
                                output_idx = 0;
                            }
                        }
                        current_letter = c;
                        current_num = 0;
                    } else {
                        idx++;
                    }
                }
            }

            //free(str);
            task_results[i].output = NULL;
            prev_task = i; 
            index = i;
            comp_tasks++;
        }
        else if (i > prev_task + 1) {
          break;
        }
    }
}

if (current_letter != '\0') {
    output_buf[output_idx++] = current_letter;
    *(unsigned char*)&output_buf[output_idx] = (unsigned char)current_num;
    output_idx += sizeof(unsigned char);
    pthread_mutex_lock(&result_mutex);
    fwrite(output_buf, 1, output_idx, stdout);
    pthread_mutex_unlock(&result_mutex);
}

//   for (int i = 0; i < num_threads; i++) {
//     pthread_join(threads[i], NULL);
// }
  
free(task_results);

  
}
     else if(num_threads == 1){
    if(argc<3){
    
     FILE *f = fopen(argv[optind], "rb");
     concat(f);
    binOutput(string, string_size);

    free(string);
    fclose(f);

  }
    else{    
    for(int i = optind; i<argc; i++){
      FILE *file1 = fopen(argv[i],"r");
      if(file1==NULL){
        exit(1);
      }
    concat(file1);
    }
     
      binOutput(string,string_size);
 
  }
     }
 
return 0;
}
