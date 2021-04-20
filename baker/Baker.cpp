// Author: Chris Ritter

#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
using namespace std;

Baker::Baker(int id):id(id) {}

Baker::~Baker() {}

void Baker::bake_and_box(ORDER &anOrder) {
	//setup the variables we'll need
	int requiredBoxes = anOrder.number_donuts / 12;
	if (anOrder.number_donuts % 12 != 0) {
		requiredBoxes++;
	}
	int donutsLeft = anOrder.number_donuts;
	
	//fill each box with donuts until the order is fulfilled
	for (int i=0; i<requiredBoxes; i++) {
		Box box;
		DONUT donut;
		bool boxHasRoom = true;
		//fill box until out of donuts or out of room
		while (donutsLeft>0 && boxHasRoom){
			boxHasRoom = box.addDonut(donut);
			if(!boxHasRoom){
				break;
			}
			donutsLeft--;
		}
		//add completed box to the order
		anOrder.boxes.push_back(box);
	}
}

void Baker::beBaker() {
	bool bake = true;
	
	while (bake){
		unique_lock<mutex> lck(mutex_order_inQ);

		if (order_in_Q.empty()) {
			if (b_WaiterIsFinished) {
				//if all the orders are done, and the waiter is done,
				//then we're done here
				bake = false;
				break;
			}
			else while (!b_WaiterIsFinished) {
				//if the waiter isn't done, but we don't have anything
				//in the queue, we need to wait
				cv_order_inQ.wait(lck);
			}
		}
		//if not, we have a queue to work with
		else {
			//fulfill the order at the front of the queue
			bake_and_box(order_in_Q.front());
			//put it on the order out vector
			order_out_Vector.push_back(order_in_Q.front());
			//then remove it from the queue
			order_in_Q.pop();
		}
		//leave everything unlocked
		lck.unlock();
	}
}
