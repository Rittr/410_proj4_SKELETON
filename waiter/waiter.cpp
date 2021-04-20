#include <string>
#include "stdlib.h"

#include "../includes/waiter.h"
#include "../includes/externs.h"

using namespace std;

Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){}

Waiter::~Waiter(){}

//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder){
	return myIO.getNext(anOrder);
}

//contains a loop that will get orders from filename one at a time
//then puts them in order_in_Q then signals baker(s) using cv_order_inQ
//so they can be consumed by baker(s)
//when finished exits loop and signals baker(s) using cv_order_inQ that
//it is done using b_WaiterIsFinished
void Waiter::beWaiter() {
	bool run = true;
	int orderStatus;

	while (run){
		ORDER currentOrder;
		orderStatus = getNext(currentOrder);

		if (orderStatus==SUCCESS){
			lock_guard<mutex> lck(mutex_order_inQ);
			order_in_Q.push(currentOrder);
		}
		else {
			//no more orders for now, wrap up
//			lock_guard<mutex> lck(mutex_order_inQ);
			b_WaiterIsFinished = true;
//			cv_order_inQ.notify_all();
			run = false;
		}
		cv_order_inQ.notify_all();
	}
}

