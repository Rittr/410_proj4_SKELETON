#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
using namespace std;

Baker::Baker(int id):id(id) {}

Baker::~Baker() {}

	//bake, box and append to anOrder.boxes vector
	//if order has 13 donuts there should be 2 boxes
	//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	int requiredBoxes = anOrder.number_donuts / 12;
	if (anOrder.number_donuts % 12 != 0) {
		requiredBoxes++;
	}
	int donutsLeft = anOrder.number_donuts;
	
	for (int i=0; i<requiredBoxes; i++) {
		Box box;
		DONUT donut;
		bool boxHasRoom = true;
		while (donutsLeft>0 && boxHasRoom){
			boxHasRoom = box.addDonut(donut);
			if(!boxHasRoom){
				break;
			}
			donutsLeft--;
		}
		anOrder.boxes.push_back(box);
	}
}

	//as long as there are orders in order_in_Q then
	//for each order:
	//	create box(es) filled with number of donuts in the order
	//  then place finished order on order_outvector
	//  if waiter is finished (b_WaiterIsFinished) then
	//  finish up remaining orders in order_in_Q and exit
	//
	//You will use cv_order_inQ to be notified by waiter
	//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
	//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
	bool bake = true;
	
	while (bake){
		unique_lock<mutex> lck(mutex_order_inQ);

		if (order_in_Q.empty()) {
			if (b_WaiterIsFinished) {
				bake = false;
				break;
			}
			else while (!b_WaiterIsFinished) {
				cv_order_inQ.wait(lck);
			}
		}
		else {
			bake_and_box(order_in_Q.front());
			order_out_Vector.push_back(order_in_Q.front());
			order_in_Q.pop();
//			lck.unlock();
		}
		lck.unlock();
	}
}
