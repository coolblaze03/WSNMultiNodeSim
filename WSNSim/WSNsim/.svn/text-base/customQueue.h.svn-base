#ifndef __CUSTOMQUEUE_H__
#define __CUSTOMQUEUE_H__
#include <queue>

using namespace std;

//not needed anymore. 
template <typename type>
class customQueue: queue<type>{

public:

	void clear(){
		while(!&queue<type>::empty){
			queue<type>::pop();
		}
	}

	void push(type const &input){
		queue<type>::push(input);
	}

	type pop(){
		type temp = queue<type>::front();
		queue<type>::pop();
		return temp;
	}

	bool empty(){
		return queue<type>::empty();
	}

};
#endif
