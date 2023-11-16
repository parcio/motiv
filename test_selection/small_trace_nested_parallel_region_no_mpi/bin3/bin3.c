#include <stdio.h>
#include <omp.h>

int main() {
  omp_set_num_threads(2);
  omp_set_nested(1);

  // Das ist unsere erste...
  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    printf("ThreadID: %d \n", id);

    omp_set_num_threads(3);

    // ...das die zweite parallele Region, *innerhalb* der ersten!
    #pragma omp parallel
    {
      int id2 = omp_get_thread_num();
 
      printf("ThreadID: %d ThreadID: %d \n", id, id2);
    }
  }
  return 0;
}

