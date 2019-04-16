#include <iostream>
#include"PathGraph.h"
#include <fstream>

#define replanW 1500

//当前道路的最大速度
int car::getCurrentRoadSpeed()
{
	return MIN(_speed, getCurrentEdge()->data->_speed);
}
//下个道路的最大行驶速度
int car::getNextRoadSpeed()
{

	return MIN(_speed, getNextEdge()->data->_speed);
	
}
//获得汽车行驶路线最大权重边
Edge* car::getPlanPathMaxWeightEdge()
{
	double maxWeiht = 0;
	int index = 0;
	
	for (int i = current_edge_index+1; i < car_new_plan_path.size(); ++i)
	{
		if (car_new_plan_path[i]->weight > maxWeiht)
		{
			maxWeiht = car_new_plan_path[i]->weight;
			index = i;
		}
	}

	return car_new_plan_path[index];

}


Edge* car::getCurrentEdge()
{
	return car_new_plan_path[current_edge_index];
}
Edge* car::getNextEdge()
{
	return car_new_plan_path[current_edge_index + 1];
}
//是否是最后的道路
bool car::isFinal()
{
	return (getCurrentEdge()->to == _to);
}
//恢复上个时间片结束的状态
void car::recover()
{
	_channel = last_channel;
	undrive_distance = last_undrive_distance;
	car_new_plan_path = last_car_new_plan_path;
	car_pass_path = last_car_pass_path;
	current_edge_index = last_current_edge_index;
	carStatus = last_carStatus;
	_start_time = last_start_time;
	lock_count = 0;
}
void car::save()
{
	last_channel = _channel;
	last_undrive_distance = undrive_distance;
	last_car_new_plan_path = car_new_plan_path;
	last_car_pass_path = car_pass_path;
	last_current_edge_index = current_edge_index;
	last_carStatus = carStatus;
	last_start_time = _start_time;
	
}



//构造
PathGraph::PathGraph()
{
	n = e = 0;
}

//析构
PathGraph::~PathGraph()
{
	n = e = 0;
}

//所有顶点、边的辅助信息复位
void PathGraph::reset()
{
	for (auto iter = v_map.begin(); iter != v_map.end();++iter)
	{
		status(iter->first) = UNDISCOVERED;
		distance(iter->first) = INT32_MAX;
		parent(iter->first) = -1;
	}
		
}
// 获得顶点i数据
std::shared_ptr<cross>& PathGraph::vertexData(int i)
{
	return v_map[i].data;
}
//i顶点的首个邻接顶点 

/*int  PathGraphfirstNbr(int i)
{
	V[i]
	 return V[i].nextNbr(i,n)
}
 //相对于顶点j的下一邻接顶点

 int  PathGraph::nextNbr(int i,int j)
 {
	 while ((-1 < j) && (!exists(i, --j)));
	 return j;
 }*/
 //获得顶点i状态

 VStatus&  PathGraph::status(int i)
 {
	 return v_map.find(i)->second.status;
 }

 //在遍历树中的父节点
 int& PathGraph::parent(int i)
 {
	 return v_map.find(i)->second.parent;
 }
 //在遍历树中的距离
 double& PathGraph::distance(int i)
 {
	 
	 return  v_map.find(i)->second.distance;
 }
//插入顶点
 void  PathGraph::insert(const std::shared_ptr<cross>& _cross)
 {
	 v_map.insert({ _cross->_cross_id ,Vertex(_cross) });
	 ++n;
 }
 //删除顶点


 //边（i，j）是否存在
 bool PathGraph::exists(int i, int j)
 {

	 return false;
 }


 //获得边(i,j)
 Edge* PathGraph::getEdge(int i, int j)
 {

	 for (auto iter = v_map[i].edgeOutVec.begin(); iter != v_map[i].edgeOutVec.end(); ++iter)
	 {

		 if ((*iter)->to == j)
		 {
			 return (*iter);
		 }
	 }
	 return NULL;
 }

 Edge* PathGraph::getEdgeByRoadIdAndFromCrossId(int roadId, int from_crossId)
 {
	 for (auto iter = v_map[from_crossId].edgeOutVec.begin(); iter != v_map[from_crossId].edgeOutVec.end(); ++iter)
	 {

		 if ((*iter)->data->_road_id == roadId)
		 {
			 return (*iter);
		 }
	 }
	 return NULL;
 }


 //获得边(i,j)的数据
 std::shared_ptr<road>& PathGraph::edgeData(int i, int j)
 {


	 for (auto iter = v_map[i].edgeOutVec.begin(); iter != v_map[i].edgeOutVec.end(); ++iter)
	 {
		 if ((*iter)->to == j)
		 {
			 return (*iter)->data;
		 }
	 }
	
 }

 //边(i,j）的权重
 double PathGraph::weight(int i, int j)
 {


	 for (auto iter = v_map[i].edgeOutVec.begin(); iter != v_map[i].edgeOutVec.end(); ++iter)
	 {
		 if ((*iter)->to == j)
		 {
			 return (*iter)->weight;
		 }
	 }

 }

 //插入一条边(i,j) 权重为w
 void PathGraph::insert(const std::shared_ptr<road>& road_data, int w, int i , int j)
 {
	// if (exists(i, j))//确认该边尚不存在
	//	 return;
	// Edge* pEdge = new Edge(road_data, w);
	 Edge* pEdge = new Edge(road_data, w);
	 pEdge->from = i;
	 pEdge->to = j;
	 v_map[i].edgeOutVec.push_back(pEdge);

	 v_map[j].edgeInVec.push_back(pEdge);

	 std::stable_sort(v_map[i].edgeOutVec.begin(), v_map[i].edgeOutVec.end(),
		 [](const Edge*  a, const Edge*  b)
	 {return a->data->_road_id < b->data->_road_id; });

	 std::stable_sort(v_map[j].edgeInVec.begin(), v_map[j].edgeInVec.end(),
		 [](const Edge*  a, const Edge*  b)
	 {return a->data->_road_id < b->data->_road_id; });
	 ++e;
 }

 void PathGraph::init(std::vector<std::shared_ptr<road>>& road_list, std::vector<std::shared_ptr<cross>>& cross_list, std::ifstream& in,std::vector<std::shared_ptr<car>>& car_list)
 {
	
	 //插入节点
	 for (auto cross_iter = cross_list.begin(); cross_iter != cross_list.end(); ++cross_iter)
	 {
		 insert(*cross_iter);
	 }
	 //std::cout << n << "\n";
	 //插入边
	 for (auto road_iter = road_list.begin(); road_iter != road_list.end(); ++road_iter)
	 {
		 if ((*road_iter)->_isDuplex == 1)
		 {
			 insert(*road_iter, 1, (*road_iter)->_from, (*road_iter)->_to);
			 insert(*road_iter, 1, (*road_iter)->_to, (*road_iter)->_from);
		 }
		 else if((*road_iter)->_isDuplex == 0)
		 {
			 insert(*road_iter, 1,(*road_iter)->_from, (*road_iter)->_to);
		 }

	 }

 }

 //为车规划路径
// void PathGraph::planningPath(std::shared_ptr<car>& mycar)
// {
//	 Dijkstra(mycar->_from, mycar->_to);
//	 int a = mycar->_to;
//	 int b;
//	 do {
//
//		 b = parent(a);
//		 Edge* pEdge = getEdge(b, a);
//		 mycar->car_new_plan_path.push_back(pEdge);
//		 a = b;
//	 } while (parent(a) != -1);
//	 std::reverse(mycar->car_new_plan_path.begin(), mycar->car_new_plan_path.end());
// }

 //处理每个道路上的车
 void PathGraph::driveAllCarJustOnRoadToEndState(Edge* edge)
 {
	 auto& vec = edge->carsInPerRoads;

	 for (int i = 0; i < vec.size(); i++)
	 {
		 for (auto iter = vec[i].begin(); iter != vec[i].end(); ++iter)
		 {

			 //是否是该车道上的第一辆车
			 if (iter == vec[i].begin())
			 {
				 //不通过路口的最大速度小于 车辆在该车道未行驶的距离 能等于
				 if ((*iter)->getCurrentRoadSpeed() <= (*iter)->undrive_distance)
				 {
					 (*iter)->undrive_distance = (*iter)->undrive_distance - (*iter)->getCurrentRoadSpeed();
					 (*iter)->carStatus = END;
				 }
				 else
				 {
					 (*iter)->carStatus = WAIT;
	
				 }
			 }
			 else
			 {


				 --iter;
				 auto iter_last = iter;//获取前面一辆车的信息
				 ++iter;
				 //比较汽车的速度 与前面一辆车的间隔 这里不能等于 否则可能出现两车重合的情况
				 if ((*iter)->getCurrentRoadSpeed() < (*iter)->undrive_distance - (*iter_last)->undrive_distance)
				 {
					 (*iter)->undrive_distance = (*iter)->undrive_distance - (*iter)->getCurrentRoadSpeed();
					 (*iter)->carStatus = END;
				 }
				 else
				 {
					 //看前面的汽车状态
					 if ((*iter_last)->carStatus == END)
					 {
						 (*iter)->undrive_distance = (*iter_last)->undrive_distance + 1;
						 (*iter)->carStatus = END;
					 }
					 else if ((*iter_last)->carStatus == WAIT)
					 {
						 (*iter)->carStatus = WAIT;
					 }
				 }
			 }
		 }
	 }
 }

 // 车 进入下一车道后 处理该车道上的车
 void PathGraph::driveAllCarJustOnLaneToEndState(Edge* edge,int channel)
 {
	 auto& vec = edge->carsInPerRoads;
	 for (auto iter = vec[channel].begin(); iter != vec[channel].end();	++iter)
	 {

		 if ((*iter)->carStatus == END)
		 {
			 continue;
		 }
		 if ((*iter)->carStatus == WAIT )//
		 {

			 //是否是该车道上的第一辆车
			 if (iter == vec[channel].begin())
			 {

				 //不通过路口的最大速度小于 车辆在该车道未行驶的距离 能等于
				 if ((*iter)->getCurrentRoadSpeed() <= (*iter)->undrive_distance)
				 {
					 (*iter)->undrive_distance = (*iter)->undrive_distance - (*iter)->getCurrentRoadSpeed();
					 (*iter)->carStatus = END;
				 }
				 else
				 {
					 (*iter)->carStatus = WAIT;
				 }
			 }
			 else
			 {
	

				 auto iter_last = --iter;//获取前面一辆车的信息
				 ++iter;
				 //比较汽车的速度 与前面一辆车的间隔 这里不能等于 否则可能出现两车重合的情况
				 if ((*iter)->getCurrentRoadSpeed() < ((*iter)->undrive_distance - (*iter_last)->undrive_distance))
				 {
					 (*iter)->undrive_distance = (*iter)->undrive_distance - (*iter)->getCurrentRoadSpeed();
					 (*iter)->carStatus = END;
				 }
				 else
				 {
					 //看前面的汽车状态
					 if ((*iter_last)->carStatus == END)
					 {
						 (*iter)->undrive_distance = (*iter_last)->undrive_distance + 1;
						 (*iter)->carStatus = END;
					 }
					 else if ((*iter_last)->carStatus == WAIT)
					 {
						 (*iter)->carStatus = WAIT;
					 }
				 }
		


			 }
		 }

		 
	 }
	

 }
 //是否存在为等待状态的车辆 //存在返回true
 bool PathGraph::existWaitCars(std::vector<std::shared_ptr<car>>& car_list)
 {
	 for (auto iter = car_list.begin(); iter != car_list.end(); ++iter)
	 {
		 if ((*iter)->carStatus == WAIT)
			 return true;
	 }
	 return false;

 }

 //这条道路处理的车
 std::shared_ptr<car> PathGraph::getCarFromRoad(Edge* edge)
 {
	 auto& vec = edge->carsInPerRoads;
	 int less_distance = 100;
	 std::shared_ptr<car> prior_car = NULL;
	 bool exist_priorityCar = false;
	 //先确定是否存在优先车辆
	 for (int i = 0; i < vec.size(); i++)
	 {
		 
		 if (!vec[i].empty())
		 {

			 if (vec[i].front()->carStatus == WAIT && vec[i].front()->_priority == 1)
			 {
				 exist_priorityCar = true;
			 }

				 
		 }
	 }

	 for (int i = 0; i < vec.size(); i++)
	 {

		 if (!vec[i].empty())
		 {
			 //存在优先车辆
			 if (exist_priorityCar)
			 {

				 if (vec[i].front()->carStatus == WAIT && vec[i].front()->_priority == 1&& vec[i].front()->undrive_distance < less_distance)
				 {
					 less_distance = vec[i].front()->undrive_distance;
					 prior_car = vec[i].front();
				 }
			 }
			 else//不存在优先车辆
			 {
				 if (vec[i].front()->carStatus == WAIT && vec[i].front()->undrive_distance < less_distance)
				 {
					 less_distance = vec[i].front()->undrive_distance;
					 prior_car = vec[i].front();
				 }
			 }
		 }
	 }
	 return prior_car;
 }

 //是否发生冲突
 bool PathGraph::isConflict(std::shared_ptr<car> myCar)
 {
	 int vertexId = myCar->getCurrentEdge()->to;


	 //优先车辆的情况
	 if (myCar->_priority == 1)
	 {

	


		 for (auto iter = v_map[vertexId].edgeInVec.begin(); iter != v_map[vertexId].edgeInVec.end(); ++iter)
		 {
			 //if ((*iter)->data->_road_id != myCar->getNextEdge()->data->_road_id && (*iter)->data->_road_id != myCar->getCurrentEdge()->data->_road_id)
			 if ((*iter)->data->_road_id != myCar->getCurrentEdge()->data->_road_id)
			 {
				 auto otherCar = getCarFromRoad((*iter));
				 if (otherCar)
				 {
					 //其余道路优先级最高的汽车也为优先车辆时
					 if (otherCar->_priority == 1)
					 {
						 if (!myCar->isFinal() && otherCar->isFinal())
						 {
							 Edge* pEdge = getStraightEdge(otherCar);
							 if (pEdge)
							 {
								 if (pEdge->data->_road_id == myCar->getNextEdge()->data->_road_id)
								 {
									 return true;
								 }
							 }

						 }
						 else if (!myCar->isFinal() && !otherCar->isFinal())
						 {
							 CarDirection myCarDirection = getDirection(myCar);
							 CarDirection otherCarDirection = getDirection(otherCar);
							 if (myCar->getNextEdge()->data->_road_id == otherCar->getNextEdge()->data->_road_id)
							 {
								 if (myCarDirection == RIGHT)
								 {

									 if (otherCarDirection == LEFT || otherCarDirection == STRAIGHT)
									 {
										 return true;
									 }

								 }
								 else if (myCarDirection == LEFT)
								 {
									 if (otherCarDirection == STRAIGHT)
									 {
										 return true;
									 }

								 }
							 }
						
						 }
					 }
					 else//其余道路优先级最高的汽车不是优先车辆时
					 { 
						 continue;
					 }
				 }
			 }
		 }
		 return false;
	 }
	 else//非优先车辆的情况
	 {
		 for (auto iter = v_map[vertexId].edgeInVec.begin(); iter != v_map[vertexId].edgeInVec.end(); ++iter)
		 {
			// if ((*iter)->data->_road_id != myCar->getNextEdge()->data->_road_id && (*iter)->data->_road_id != myCar->getCurrentEdge()->data->_road_id)
			 if ((*iter)->data->_road_id != myCar->getCurrentEdge()->data->_road_id)
			 {
				 auto otherCar = getCarFromRoad((*iter));
				 if (otherCar)
				 {
					 if (otherCar->_priority == 1)//其他道路的车为优先车辆
					 {
						 if (myCar->isFinal() && !otherCar->isFinal())
						 {
							 Edge* pEdge = getStraightEdge(myCar);
							 if (pEdge)
							 {
								 if (pEdge->data->_road_id == otherCar->getNextEdge()->data->_road_id)
								 {
									 return true;
								 }
							 }

						 }
						 else if (!myCar->isFinal() && otherCar->isFinal())
						 {
							 Edge* pEdge = getStraightEdge(otherCar);
							 if (pEdge)
							 {
								 if (pEdge->data->_road_id == myCar->getNextEdge()->data->_road_id)
								 {
									 return true;
								 }
							 }
						 }
						 else if (!myCar->isFinal() && !otherCar->isFinal())
						 {
							 if (otherCar->getNextEdge()->data->_road_id == myCar->getNextEdge()->data->_road_id)
							 {
								 return true;
							 }
						 }
					 }
					 else//其他道路的车也为非优先车辆
					 {
						 if (!myCar->isFinal() && otherCar->isFinal())
						 {
							 Edge* pEdge = getStraightEdge(otherCar);
							 if (pEdge)
							 {
								 if (pEdge->data->_road_id == myCar->getNextEdge()->data->_road_id)
								 {
									 return true;
								 }
							 }

						 }
						 else if (!myCar->isFinal() && !otherCar->isFinal())
						 {
							 CarDirection myCarDirection = getDirection(myCar);
							 CarDirection otherCarDirection = getDirection(otherCar);
							 if (myCar->getNextEdge()->data->_road_id == otherCar->getNextEdge()->data->_road_id)
							 {
								 if (myCarDirection == RIGHT)
								 {

									 if (otherCarDirection == LEFT || otherCarDirection == STRAIGHT)
									 {
										 return true;
									 }

								 }
								 else if (myCarDirection == LEFT)
								 {
									 if (otherCarDirection == STRAIGHT)
									 {
										 return true;
									 }

								 }
							 }
						 }
					 }

				 }
			 }
		 }
		 return false;
	 }

 }


 void PathGraph::saveAll(std::vector<std::shared_ptr<car>>& car_list)
 {
	 for (int i = 0; i < car_list.size(); ++i)
	 {

		 car_list[i]->save();

	 }


	 for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
	 {

		 for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
		 {
			 (*iter)->save();;

		 }
	 }
 }

 //重新规划后 恢复所有的路况信息
 void PathGraph::recoverAll(std::vector<std::shared_ptr<car>>& car_list)
 {

	 for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
	 {

		 for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
		 {
			 (*iter)->recover();;

		 }
	 }
	 for (int i = 0; i < car_list.size(); ++i)
	 {
		
			 car_list[i]->recover();
	 }
 }


 void PathGraph::moveToNextRoad(std::shared_ptr<car>& myCar,int time)
 {

	 //下个路口就是目的地
	 if (myCar->isFinal())
	 {
		 Edge* edge = myCar->getCurrentEdge();
		 edge ->carsInPerRoads[myCar->_channel].pop_front();//去除车子所在当前道路的链表
		 myCar->_end_time = time;
		 myCar->_channel = -1;
		 myCar->carStatus = FINISH;
		// edge->updateSelfWeight();
		 return;
	 }
	 else
	 {
		 Edge* nextEdge = myCar->getNextEdge();
		 Edge* currentEdge = myCar->getCurrentEdge();
		 auto& vecCars = nextEdge->carsInPerRoads;
		 if (myCar->getNextRoadSpeed() <= myCar->undrive_distance)
		 {
			 //这是下个道路的最大行驶速度 小于 当前道路未行驶的距离的情况
			 myCar->undrive_distance = 0;
			 myCar->carStatus = END;
			 myCar->lock_count = 0;
			// myCar->car_lock_road.clear();
			// myCar->allowRePlanPath = false; //不允许重新规	划路线
			 return;
		 }

		 for (size_t i = 0; i < vecCars.size(); i++)
		 {
			 //下个车道无车
			 if (vecCars[i].empty())
			 {
				 if (myCar->getNextRoadSpeed() > myCar->undrive_distance)
				 {
					 myCar->undrive_distance = nextEdge->data->_length - (myCar->getNextRoadSpeed() - myCar->undrive_distance);

			
					 myCar->getCurrentEdge()->carsInPerRoads[myCar->_channel].pop_front();//去除车子所在当前道路的链表

					 nextEdge->carsInPerRoads[i].push_back(myCar);//加入下个道路的链表

					 myCar->carStatus = END;
					 myCar->_channel = i;
					 ++(myCar->current_edge_index);//到下个道路去

					 myCar->lock_count = 0;

					 return;
				 }
			 }
			 else
			 {
				 
				//当车道最后面的车为等待状态时 两种情况
				 //1.没有阻挡后方来车 
				 //2.阻挡了后方来车
				 if (vecCars[i].back()->carStatus == WAIT)
				 {
					 //等待状态
					 //阻挡了
					 if ((nextEdge->data->_length - vecCars[i].back()->undrive_distance) <= (myCar->getNextRoadSpeed() - myCar->undrive_distance))
					 {
						 //若状态一直没发生变化则说明死锁
						// if (myCar->_preset != 1)
						// {
							 ++myCar->lock_count;
						// }
						 

						 return;
					 }
					 else//未阻挡
					 {
						 myCar->undrive_distance = nextEdge->data->_length - (myCar->getNextRoadSpeed() - myCar->undrive_distance);

						 nextEdge->carsInPerRoads[i].push_back(myCar);//加入下个道路的链表
						 myCar->getCurrentEdge()->carsInPerRoads[myCar->_channel].pop_front();//去除车子所在当前道路的链表

						 myCar->carStatus = END;
						 myCar->_channel = i;
						 ++(myCar->current_edge_index);
						 myCar->lock_count = 0;

						 return;
					 }
				 }
				 else if (vecCars[i].back()->carStatus == END)
				 {						/*当车道最后面的车为终止状态时 三种情况
										1.没有阻挡后方来车
										 2.阻挡了后方来车且为最后一排
										3.阻挡了后方来车不为最后一排*/

						//不在最后一排
					 if (vecCars[i].back()->undrive_distance < nextEdge->data->_length - 1)
					 {
						 //进入下一条街道要行驶的距离超过了下个车道最后一辆车的行驶距离
						 if ((nextEdge->data->_length - vecCars[i].back()->undrive_distance) <= (myCar->getNextRoadSpeed() - myCar->undrive_distance))
						 {
							 myCar->undrive_distance = vecCars[i].back()->undrive_distance + 1;
						 }
						 else//未超过
						 {
							 myCar->undrive_distance = nextEdge->data->_length - (myCar->getNextRoadSpeed() - myCar->undrive_distance);
						 }

						 nextEdge->carsInPerRoads[i].push_back(myCar);//加入下个道路的链表
						 myCar->getCurrentEdge()->carsInPerRoads[myCar->_channel].pop_front();//去除车子所在当前道路的链表

						//将下个道路加入到车子已经走的路线中
						// myCar->car_pass_path.push_back(nextEdge);


						 myCar->carStatus = END;
						 myCar->_channel = i;
						 ++(myCar->current_edge_index);
						
						 myCar->lock_count = 0;

						 return;

					 }

				 }

			 }

		 }
		 //最后一排全满
		 myCar->lock_count = 0;
		 myCar->undrive_distance = 0;
		 myCar->carStatus = END;
	 }
	 


 }

 // 所有的车都不是unstart状态 说明调度完成
 bool PathGraph::allCarFinish(std::vector<std::shared_ptr<car>>& car_list)
 {
	 for (auto iter = car_list.begin(); iter != car_list.end(); ++iter)
	 {
		 if ((*iter)->carStatus != FINISH)
			 return false;
	 }
	 return true;
 }

 //将车库中的车上路
 void PathGraph::driveCarInGarage(Edge* edge,int time,bool priority)
 {
	 auto&  ready_cars = edge->ready_cars;
	 auto& vecCars = edge->carsInPerRoads;
	 
	
	 for (auto iter = ready_cars.begin(); iter != ready_cars.end(); ++iter)
	 {
		 if ((*iter)->_start_time <= time)
		 {
			 if (priority == true)
			 {
				 if ((*iter)->_priority == 1)
				 {
					 for (int i = 0; i < vecCars.size(); i++)
					 {
						 //下个车道无车
						 if (vecCars[i].empty())
						 {
							 if ((*iter)->getNextRoadSpeed() > (*iter)->undrive_distance)
							 {
								 (*iter)->undrive_distance = edge->data->_length - (*iter)->getNextRoadSpeed();
								 edge->carsInPerRoads[i].push_back((*iter));//加入下个道路的链表
								// (*iter)->_start_time = time;
								 (*iter)->carStatus = END;
								 (*iter)->_channel = i;
								 ++((*iter)->current_edge_index);//到下个道路去

								 (*iter)->lock_count = 0;
								 								 
								 iter = ready_cars.erase(iter);//删除后返回下个元素的迭代器
								 --iter;
								 break;
							 }
						 }
						 else
						 {

							 //当车道最后面的车为等待状态时 两种情况
							  //1.没有阻挡后方来车 
							  //2.阻挡了后方来车
							 if (vecCars[i].back()->carStatus == WAIT)
							 {
								 //等待状态
								 //阻挡了
								 if ((edge->data->_length - vecCars[i].back()->undrive_distance) <= (*iter)->getNextRoadSpeed())
								 {
									 break;
								 }
								 else//未阻挡
								 {
									 (*iter)->undrive_distance = edge->data->_length - (*iter)->getNextRoadSpeed();

									 edge->carsInPerRoads[i].push_back((*iter));//加入下个道路的链表

									 (*iter)->carStatus = END;
									 (*iter)->_channel = i;
									 ++((*iter)->current_edge_index);

									 (*iter)->lock_count = 0;

									 iter = ready_cars.erase(iter);//删除后返回下个元素的迭代器
									 --iter;
									 break;
								 }
							 }
							 else if (vecCars[i].back()->carStatus == END)
							 {						/*当车道最后面的车为终止状态时 三种情况
													1.没有阻挡后方来车
													 2.阻挡了后方来车且为最后一排
													3.阻挡了后方来车不为最后一排*/

													//不在最后一排
								 if (vecCars[i].back()->undrive_distance < edge->data->_length - 1)

								 {
									 //进入下一条街道要行驶的距离超过了下个车道最后一辆车的行驶距离
									 if ((edge->data->_length - vecCars[i].back()->undrive_distance) <= (*iter)->getNextRoadSpeed())
									 {
										 (*iter)->undrive_distance = vecCars[i].back()->undrive_distance + 1;
									 }
									 else//未超过
									 {
										 (*iter)->undrive_distance = edge->data->_length - (*iter)->getNextRoadSpeed();
									 }

									 edge->carsInPerRoads[i].push_back((*iter));//加入下个道路的链表

									 (*iter)->carStatus = END;
									 (*iter)->_channel = i;
									 ++((*iter)->current_edge_index);

									 (*iter)->lock_count = 0;

									 iter = ready_cars.erase(iter);//删除后返回下个元素的迭代器
									 --iter;
									 break;

								 }

							 }

						 }

					 }
				 }
				 else
				 {
					 continue;
				 }
			 }
			 else
			 {
				 for (int i = 0; i < vecCars.size(); i++)
				 {
					 //下个车道无车
					 if (vecCars[i].empty())
					 {

						 (*iter)->undrive_distance = edge->data->_length - (*iter)->getNextRoadSpeed();

						 edge->carsInPerRoads[i].push_back((*iter));//加入下个道路的链表
						 (*iter)->carStatus = END;
						 (*iter)->_channel = i;
						 ++((*iter)->current_edge_index);//到下个道路去

						 (*iter)->lock_count = 0;

						 iter = ready_cars.erase(iter);//删除后返回下个元素的迭代器
						 --iter;
						 break;
					 }
					 else
					 {

						 if (vecCars[i].back()->undrive_distance < edge->data->_length - 1)
						 {
							 //进入下一条街道要行驶的距离超过了下个车道最后一辆车的行驶距离
							 if ((edge->data->_length - vecCars[i].back()->undrive_distance) <= (*iter)->getNextRoadSpeed())
							 {
								 (*iter)->undrive_distance = vecCars[i].back()->undrive_distance + 1;
							 }
							 else//未超过
							 {
								 (*iter)->undrive_distance = edge->data->_length - (*iter)->getNextRoadSpeed();
							 }

							 edge->carsInPerRoads[i].push_back((*iter));//加入下个道路的链表

							 (*iter)->carStatus = END;
							 (*iter)->_channel = i;
							 ++((*iter)->current_edge_index);

							 (*iter)->lock_count = 0;

							 iter = ready_cars.erase(iter);//删除后返回下个元素的迭代器
							 --iter;
							 break;

						 }



					 }

				 }

			 }
		 } 
		
	 }


 }

//开始调度
 void PathGraph::start(std::vector<std::shared_ptr<car>>& car_list)
 {
	 std::ofstream fout;
	 fout.open("compare.txt", std::ios::out);

	 

	 int time = 0;



	 while (true)
	 {


		 ++time;//时间片加一

		 
		//为每条车道的车库里的车排序  1.车辆优先级 2.车辆出发时间  3.车辆ID
		 for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
		 {

			 for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
			 {
				 (*iter)->putReadyCarInGarage(time);
			 }
		 }

		 //处理每条道路的车
		 for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
		 {	

			 for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
			 {
				 driveAllCarJustOnRoadToEndState((*iter));
			 }
		 }
		 //优先上路车辆
		
		 for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
		 {

			 for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
			 {
				 
				 driveCarInGarage((*iter),time,true);
			 }
	
		 }

TEST:	 while (existWaitCars(car_list))
		 {
			 //处理每个路口
				for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
				{
			

					 for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
					 {

						 //对一个道 路反复调度 一直到无车辆可调度或者处于冲突状态
						 while (true)
						 {
							 auto myCar = getCarFromRoad((*iter));

							 if (myCar)
							 {
								 int carChannel = myCar->_channel;
								 //在这个路口 如果存在冲突 调度下一个道路 
								 if (isConflict(myCar))
								 {
									 break;
								 }
								 //不存在冲突 汽车要么去下一个道路 要么在这个道路的最前方(未行驶距离)
								 moveToNextRoad(myCar, time);
								 if (myCar->carStatus == WAIT)
								 {
									 if (myCar->lock_count > 30)
									 {
										 std:: cout<< "deadLock,crossId::" << myCar->getCurrentEdge()->to << "\n";
									 }
									 
									 break;
								 }
								 //并且将该道路的所有车进行一次调度
								 driveAllCarJustOnLaneToEndState((*iter), carChannel);
								 driveCarInGarage((*iter), time, true);

							 }
							 else
							 {
								 break;
							 }
						 }
					 }
				}
		}
		if (allCarFinish(car_list))// 所有的车都不是unstart状态 说明调度完成
		{
			calculateTime(car_list,time);
			break;
		}
		//updateWeight();
		//所有未上路车辆
		for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
		{
			
			for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
			{
				driveCarInGarage((*iter), time, false);
			}
		}	


		fout << "time:" << time << "\n";
		for (auto iter_vertex = v_map.begin(); iter_vertex != v_map.end(); ++iter_vertex)
		{

			for (auto iter = iter_vertex->second.edgeInVec.begin(); iter != iter_vertex->second.edgeInVec.end(); ++iter)
			{
				fout << "roadId:" << (*iter)->data->_road_id << ",from:" << (*iter)->from << ",to:" << (*iter)->to << "\n";
				auto& veccars = (*iter)->carsInPerRoads;
				for (int i = 0; i < veccars.size(); i++)
				{
					fout << "车道" << i << ": " << "\n";
					for (auto iter_car = veccars[i].begin(); iter_car != veccars[i].end(); ++iter_car)
					{
						fout << (*iter_car)->_car_id << " : " << (*iter_car)->undrive_distance << ",";
					}
					fout << "\n";
				}
			}
		}
		fout.flush();

	 }
	
	
 }

 void calculateTime(std::vector<std::shared_ptr<car>>& car_list,int time)
 {
	 int prior_start_time = INT32_MAX;
	 int prior_end_time = 0;
	 int prior_total = 0;
	 int total_time = 0;

	 int car_count = 0;
	 int prior_car_count = 0;
	 std::set<int> start_place;
	 std::set<int> end_place;

	 std::set<int> prior_start_place;
	 std::set<int> prior_end_place;

	 int high_speed = 0;
	 int low_speed = INT32_MAX;

	 int prior_high_speed = 0;
	 int prior_low_speed = INT32_MAX;

	 int car_early_start_time = INT32_MAX;
	 int prior_car_early_start_time = INT32_MAX;

	 int car_late_start_time = 0;
	 int prior_car_late_start_time = 0;
	 for (auto iter = car_list.begin(); iter != car_list.end(); ++iter)
	 {

		 if ((*iter)->_priority == 1)
		 {
			 prior_car_count++;
			 prior_start_place.insert((*iter)->_from);
			 prior_end_place.insert((*iter)->_to);
			 if ((*iter)->_plantime < prior_start_time)
			 {
				 prior_start_time = (*iter)->_plantime;
			 }
			 if ((*iter)->_end_time > prior_end_time)
			 {
				 prior_end_time = (*iter)->_end_time;
			 }

			 if ((*iter)->_speed < prior_low_speed)
			 {
				 prior_low_speed = (*iter)->_speed;
			 }
			 if ((*iter)->_speed > prior_high_speed)
			 {
				 prior_high_speed = (*iter)->_speed;
			 }
			 if ((*iter)->_plantime < prior_car_early_start_time)
			 {
				 prior_car_early_start_time = (*iter)->_plantime;
			 }

			 if ((*iter)->_plantime > prior_car_late_start_time)
			 {
				 prior_car_late_start_time = (*iter)->_plantime;
			 }

			 prior_total += ((*iter)->_end_time - (*iter)->_plantime);
		 }


		 car_count++;
		 start_place.insert((*iter)->_from);
		 end_place.insert((*iter)->_to);
		 if ((*iter)->_speed < low_speed)
		 {
			 low_speed = (*iter)->_speed;
		 }
		 if ((*iter)->_speed > high_speed)
		 {
			 high_speed = (*iter)->_speed;
		 }
		 if ((*iter)->_plantime < car_early_start_time)
		 {
			 car_early_start_time = (*iter)->_plantime;
		 }
		 if ((*iter)->_plantime > car_late_start_time)
		 {
			 car_late_start_time = (*iter)->_plantime;
		 }



		 total_time += ((*iter)->_end_time - (*iter)->_plantime);
	 }
	 float a = 0.05*car_count / prior_car_count + 0.2375*high_speed*prior_low_speed / low_speed / prior_high_speed
		 + 0.2375*car_late_start_time*prior_car_early_start_time / prior_car_late_start_time / car_early_start_time
		 + 0.2375*start_place.size() / prior_start_place.size() + 0.2375*end_place.size() / prior_end_place.size();


	 float b = 0.8*car_count / prior_car_count + 0.05*high_speed*prior_low_speed / low_speed / prior_high_speed
		 + 0.05*car_late_start_time*prior_car_early_start_time / prior_car_late_start_time / car_early_start_time
		 + 0.05*start_place.size() / prior_start_place.size() + 0.05*end_place.size() / prior_end_place.size();

	 std::cout << "a = " << a << "\n";
	 std::cout << "b = " << b << "\n";
	 std::cout << "specialResult is Result{ scheduleTime = " << prior_end_time - prior_start_time << ", allScheduleTime = " << prior_total << "\n";

	 std::cout << "originResult is Result{ scheduleTime = " << time << ", allScheduleTime = " << total_time << "\n";

	 std::cout << "CodeCraftJudge end schedule time is " << (int)(a*(prior_end_time - prior_start_time) + time + 0.5) << " allScheduleTime is " <<
		 (int)(b*prior_total + total_time + 0.5) << "\n";


 }

 //获取直行的edge
 Edge* PathGraph::getStraightEdge(std::shared_ptr<car>& myCar)
 {
	 if (myCar->getCurrentEdge()->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_left_road_id)
	 {
		 for (auto iter = v_map[myCar->getCurrentEdge()->to].edgeOutVec.begin(); iter != v_map[myCar->getCurrentEdge()->to].edgeOutVec.end(); ++iter)
		 {
			 if ((*iter)->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_right_road_id)
			 {
				 return *iter;
			 }
		 }
	 }
	 else if (myCar->getCurrentEdge()->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_buttom_road_id)
	 {
		 for (auto iter = v_map[myCar->getCurrentEdge()->to].edgeOutVec.begin(); iter != v_map[myCar->getCurrentEdge()->to].edgeOutVec.end(); ++iter)
		 {
			 if ((*iter)->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_top_road_id)
			 {
				 return *iter;
			 }
		 }
	 }
	 else if (myCar->getCurrentEdge()->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_right_road_id)
	 {
		 for (auto iter = v_map[myCar->getCurrentEdge()->to].edgeOutVec.begin(); iter != v_map[myCar->getCurrentEdge()->to].edgeOutVec.end(); ++iter)
		 {
			 if ((*iter)->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_left_road_id)
			 {
				 return *iter;
			 }
		 }
	 }
	 else if (myCar->getCurrentEdge()->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_top_road_id)
	 {
		 for (auto iter = v_map[myCar->getCurrentEdge()->to].edgeOutVec.begin(); iter != v_map[myCar->getCurrentEdge()->to].edgeOutVec.end(); ++iter)
		 {
			 if ((*iter)->data->_road_id == v_map[myCar->getCurrentEdge()->to].data->_buttom_road_id)
			 {
				 return *iter;
			 }
		 }
	 }
	 return NULL;
 }
 CarDirection PathGraph::getDirection(std::shared_ptr<car> mycar)
 {
	 if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_left_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_buttom_road_id)
	 {
		 return RIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_left_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_right_road_id)
	 {
		 return STRAIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_left_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_top_road_id)
	 {
		 return LEFT;
	 }

	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_buttom_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_right_road_id)
	 {
		 return RIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_buttom_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_top_road_id)
	 {
		 return STRAIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_buttom_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_left_road_id)
	 {
		 return LEFT;
	 }

	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_right_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_top_road_id)
	 {
		 return RIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_right_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_left_road_id)
	 {
		 return STRAIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_right_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_buttom_road_id)
	 {
		 return LEFT;
	 }

	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_top_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_left_road_id)
	 {
		 return RIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_top_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_buttom_road_id)
	 {
		 return STRAIGHT;
	 }
	 else if (mycar->getCurrentEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_top_road_id && mycar->getNextEdge()->data->_road_id == v_map[mycar->getCurrentEdge()->to].data->_right_road_id)
	 {
		 return LEFT;
	 }


 }
 void PathGraph::readAnswer(std::ifstream& in, std::vector<std::shared_ptr<car>>& car_list)
 {
	 std::string read_data;
	 int preset_start_time;
	 //int preset_plan_road;
	 int preset_carId;
	 while (std::getline(in, read_data))
	 {
		 if (read_data.find('#') == -1)
		 {
			 size_t a = read_data.find('(');
			 std::string data = read_data.substr(a + 1, read_data.size() - 2);//去掉（）
			 std::replace(data.begin(), data.end(), ',', ' ');//用空格代替,
			 std::istringstream iss(data);//字符流
			 std::string item;

			 iss >> item;
			 preset_carId = std::stoi(item);

			 iss >> item;
			 preset_start_time = std::stoi(item);


			 //快速查找
			 auto iter = std::find_if(car_list.begin(), car_list.end(),
				 [preset_carId](const std::shared_ptr<car>  &a)
			 {return a->_car_id == preset_carId; });
			 
			 if (!(*iter)->car_new_plan_path.empty())
			 {
				 continue;
			 }//复赛现场添加的部分 读取修改了路径的预置车辆
			 
			 (*iter)->_start_time = preset_start_time;
			 int cross_from = (*iter)->_from;

			 while (iss >> item)
			 {
				 Edge* pEdge = getEdgeByRoadIdAndFromCrossId(std::stoi(item), cross_from);
				 (*iter)->car_new_plan_path.push_back(pEdge);
				 cross_from = pEdge->to;

			 }
			 //将presetCar 提前置入 每条道路的车库中
			 Edge* pEdge = getEdgeByRoadIdAndFromCrossId((*iter)->getNextEdge()->data->_road_id, (*iter)->_from);
			 pEdge->garage_cars.push_back((*iter));
			 (*iter)->isInGarage = InEdgeGarage;
		 }
	 }
 }
