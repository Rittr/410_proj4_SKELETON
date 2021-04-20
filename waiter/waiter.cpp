// Author: Chris Ritter

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

void Waiter::beWaiter() {
	bool run = true;
	int orderStatus;

	while (run){
		ORDER currentOrder;
		//variable used to see if there are still more orders
		orderStatus = getNext(currentOrder);

		if (orderStatus==SUCCESS){
			lock_guard<mutex> lck(mutex_order_inQ);
			//add order to queue
			order_in_Q.push(currentOrder);
		}
		else {
			//no more orders, wrap up
			b_WaiterIsFinished = true;
			run = false;
		}
		//notify all for multiple bakers
		cv_order_inQ.notify_all();
	}
}

