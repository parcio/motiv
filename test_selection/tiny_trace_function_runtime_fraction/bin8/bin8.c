#include <stdio.h>
#include <omp.h>

void func_10(){
	sleep(1);
}
void func_20(){
	sleep(2);
}
void func_30(){
	sleep(3);
}
void func_40(){
	sleep(4);
}

int main() {
  omp_set_num_threads(4);

  // Das ist unsere erste...
  #pragma omp parallel
  {
    	int id = omp_get_thread_num();
	if(id==0){
		func_10();
		func_20();
		func_30();
		func_40();
	}
	if(id==1){
		func_20();
		func_20();
		func_30();
		func_30();
	}
	if(id==2){
		func_20();
		func_20();
		func_20();
		func_40();
	}
	if(id==3){
		func_20();
		func_40();
		func_40();
	}
  }
  return 0;
}

