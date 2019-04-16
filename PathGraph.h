#include<map>
#include<list>
#include<string>
#include<queue>
#include<memory>
#include<algorithm>
#include<cstdint>
#include<math.h>
#include<cmath>
#include <utility>
#include<set>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
typedef enum {UNDISCOVERED,DISCOVERED,VISITED} VStatus ;
typedef enum { UNSTART, START,WAIT,END,FINISH } CarStatus;
typedef enum { STRAIGHT, LEFT, RIGHT } CarDirection;
typedef enum {NotInEdgeGarage,InEdgeGarage }CarIsInEdgeGarage;
#define MIN(A,B) (A)>(B)?(B):(A)

struct Edge;
class car
{
public:
	car(int car_id, int from, int to, int speed, int plantime,int priority,int preset)
		:_car_id(car_id), _from(from), _to(to), _speed(speed), _plantime(plantime), _priority(priority), _preset(preset)
		,_start_time(-1),_end_time(-1) ,carStatus(UNSTART), undrive_distance(0), _channel(-1), current_edge_index(-1), lock_count(0), allowRePlanPath(true), isInGarage(NotInEdgeGarage)
		
	{

	}
	//获得汽车行驶路线最大权重边
	Edge* getPlanPathMaxWeightEdge();

		//当前道路的最大速度
	int getCurrentRoadSpeed(void);
	//void recoverOldPath();
	//下个道路的最大行驶速度
	int getNextRoadSpeed(void);
	Edge* getCurrentEdge();
	Edge* getNextEdge();
	//是否是最后的道路
	bool isFinal();
	void recover();
	void save();
	CarStatus carStatus;
	int _car_id;//汽车Id
	int _end_time;//汽车结束时间
	int _from;//从哪个路口出发
	int _to;//到哪个路口停止
	int _speed;//汽车行驶最大速度
	int _plantime;//最小出发时间
	int _priority;//是否优先
	int _preset;//是否为预置车辆

	int _start_time;//实际出发时间
	int _channel;//所在车道
	int undrive_distance;//车辆在当前道路还未行驶的距离

	int lock_count;//死锁时 每次等待+1
	CarIsInEdgeGarage isInGarage;
	bool allowRePlanPath;//如果车子当前时间片未能成功过路口且成为终止状态后 则不允许重新规划道路
	std::vector< Edge* > car_lock_road;//存放导致死锁的道路




	std::vector< Edge* > car_new_plan_path;//存放车辆新的规划路径
	//std::vector< Edge* > car_old_plan_path;//存放重新规划之前的路径
	std::vector<Edge*> car_pass_path;//存放车辆已经行驶的路径
	int current_edge_index;
	//int old_edge_index;



	int last_channel;
	int last_undrive_distance;
	std::vector< Edge* >  last_car_new_plan_path;
	std::vector<Edge*> last_car_pass_path;//存放车辆已经行驶的路径
	int  last_current_edge_index;
	CarStatus last_carStatus;
	int last_start_time;

};
class road 
{
public:
	road(int road_id, int length, int speed, int channel, int from, int to, int isDuplex)
		:_road_id(road_id), _length(length), _speed(speed), _channel(channel), _from(from), _to(to), _isDuplex(isDuplex)
	{
		
	}

	int _road_id;//道路Id号
	int _length;//道路长度
	int _speed;//道路允许的最大速度
	int _channel;//道路的车道数
	int _from;//连接的路口
	int _to;//连接的路口
	int _isDuplex;//是否双向

};


class cross
{
	public:
	cross(int cross_id,int right_road_id,int buttom_road_id,int left_road_id,int top_road_id)
		:_cross_id(cross_id), _right_road_id(right_road_id), _buttom_road_id(buttom_road_id), _left_road_id(left_road_id), _top_road_id(top_road_id)
	{}
	int _cross_id;//路口Id
	int _right_road_id;//右道路Id
	int _buttom_road_id;//下道路Id
	int _left_road_id;//左道路Id
	int _top_road_id;//上道路Id
	//std::list<car> notDriveCars;
}; 

//边结构
struct Edge
{
	//注意这里和road的不一样
	int from;//边的起点
	int to;//边的终点点
	std::shared_ptr<road> data;//每条路中的数据

	//std::set<CarDirection> car_direction_set;
	void updateSelfWeight()
	{

		double car_count = 0;
		double road_channel = data->_channel;
		double road_length = data->_length;

		for (int i = 0; i < carsInPerRoads.size(); i++)
		{
			car_count += carsInPerRoads[i].size();
		}

		weight = (1+ car_count / (road_channel*road_length)) * 1000;
	

	}

	void recover()
	{
		weight = last_weight;
		carsInPerRoads = last_carsInPerRoads;
	}
	void save()
	{

		last_weight = weight;
		last_carsInPerRoads = carsInPerRoads;
	}
	void sortCarsInGarage()
	{
		std::vector< std::shared_ptr<car>> vec_cars = std::vector< std::shared_ptr<car>>(ready_cars.begin(), ready_cars.end());

		//车辆ID
		std::stable_sort(vec_cars.begin(), vec_cars.end(),
			[](const std::shared_ptr<car>  &a, const std::shared_ptr<car>  &b)
		{return a->_car_id < b->_car_id; });

	    //发车时间
		std::stable_sort(vec_cars.begin(), vec_cars.end(),
			[](const std::shared_ptr<car>  &a, const std::shared_ptr<car>  &b)
		{return a->_start_time < b->_start_time; });
		//优先级
		std::stable_sort(vec_cars.begin(), vec_cars.end(),
			[](const std::shared_ptr<car>  &a, const std::shared_ptr<car>  &b)
		{return a->_priority > b->_priority; });

		ready_cars = std::list< std::shared_ptr<car>>(vec_cars.begin(), vec_cars.end());
	}
	
	void putReadyCarInGarage(int time)
	{
		bool change = false;
		for (auto iter = garage_cars.begin(); iter != garage_cars.end(); ++iter)
		{

			if ((*iter)->_start_time <= time)
			{
				change = true;
				ready_cars.push_back((*iter));
				iter = garage_cars.erase(iter);
				--iter;
			}
			
		}
		if (change)
		{
			sortCarsInGarage();
		}
	}



	double weight;//边的权重
	std::vector<std::list<std::shared_ptr<car>>> carsInPerRoads;

	double last_weight;//上个时间片 边的权重
	std::vector<std::list<std::shared_ptr<car>>> last_carsInPerRoads;

	std::list<std::shared_ptr<car>> garage_cars;

	std::list<std::shared_ptr<car>> ready_cars;
	Edge(const std::shared_ptr<road>& d, int w) :data(d), weight(w)
	{
		carsInPerRoads = std::vector < std::list<std::shared_ptr<car>>>(data->_channel);
	}


};




//顶点结构
struct Vertex
{
	std::shared_ptr<cross> data;//每个路口中的数据
	VStatus status;
	int parent;//遍历树中的父节点，在最短路径中可用到
	//int priority;//这个可以代表每个路口的拥挤程度，优化的时候可以使用
	double distance;//D算法的距离
	//Edge* pFirstEdgeOut;//从顶点向外的第一条边
	//Edge* pFirstEdgeIn;//从顶点向外的第一条边

//	std::vector<std::shared_ptr<Edge>> edgeInVec;
//	std::vector<std::shared_ptr<Edge>> edgeOutVec;
	std::vector<Edge*> edgeInVec;
	std::vector<Edge*> edgeOutVec;
	//int road_count;//连接道路的个数
	Vertex() {}
	Vertex(const std::shared_ptr<cross>& d) :data(d), parent(-1), status(UNDISCOVERED)//, pFirstEdgeOut(NULL), pFirstEdgeIn(NULL)
	{
	}
};



class PathGraph
{
public:
	PathGraph();//构造
	~PathGraph();//析构

	void reset();//所有顶点、边的辅助信息复位
	std::shared_ptr<cross>& vertexData(int);//获得顶点数据

	int firstNbr(int);//首个邻接顶点 

	int nextNbr(int, int);//相对于一邻接顶点的下一顶点
	VStatus& status(int);//获得顶点状态
	int& parent(int);//在遍历树中的父节点
	double& distance(int);//在遍历树中的距离
	void insert(const std::shared_ptr<cross>&);//插入顶点	
	void remove(const std::shared_ptr<cross>&);//删除顶点

	bool exists(int, int);//边（i，j）是否存在

	std::shared_ptr<road>& edgeData(int, int);//获得边(i,j)的数据
	Edge* getEdge(int, int);//获得边(i,j)
	Edge* getStraightEdge(std::shared_ptr<car>&); //获取直行的edge
	Edge* getEdgeByRoadIdAndFromCrossId(int roadId,int from_crossId);//用道路Id和from的路口获取边
	double weight(int, int);//边(i,j）的权重

	void insert(const std::shared_ptr<road>&, int, int, int);//插入一条边(i,j)

	void remove(int, int);//删除一条边(i,j)
	void driveAllCarJustOnRoadToEndState(Edge* edge);//处理每个道路上的车

	void driveAllCarJustOnLaneToEndState(Edge* edge,int);//处理每个道路上的车

	void driveCarInGarage(Edge* edge,int,bool);//将车库中的车上路

	void init(std::vector<std::shared_ptr<road>>&, std::vector<std::shared_ptr<cross>>&, std::ifstream& ,std::vector<std::shared_ptr<car>>&);
	void start(std::vector<std::shared_ptr<car>>&);//让汽车开始跑
	//从顶点i到j的最短路径
	void Dijkstra(int i, int j)
	{
		reset();
		std::priority_queue<P, std::vector<P>, std::greater<P>> Q;
		Q.push(P(0.0, i));
		v_map.at(i).distance = 0;
		while (!Q.empty())
		{
			int u = Q.top().second;
			Q.pop();

			for (auto iter = v_map.at(u).edgeOutVec.begin(); iter != v_map.at(u).edgeOutVec.end(); ++iter)
			{
				int v = (*iter)->to;
				int _weight = 0;
				if (distance(v) > distance(u) + weight(u, v))
				{
					distance(v) = distance(u) + weight(u, v);
					//printf("%d\n", weight(u, v));
					parent(v) = u;
					Q.push(P(distance(v), v));
				}

			}
		}
	}
	void planningPath(std::shared_ptr<car>& mycar);//为车规划路径 

	 //重新规划路径
	void rePlanningPath(std::shared_ptr<car>& mycar);


	//void planningPathTest(std::shared_ptr<car>& mycar);

	bool existWaitCars(std::vector<std::shared_ptr<car>>&);//是否存在等待汽车
	CarDirection getDirection(std::shared_ptr<car>);//获得汽车下个行驶转弯方向
	void moveToNextRoad(std::shared_ptr<car>&,int);//去下一个街道

	std::shared_ptr<car> getCarFromRoad(Edge*);

	//车库中的普通车辆上路行驶
	//void driveNormalCarInGarage(std::vector<std::shared_ptr<car>>&,int time);

	void planningPathAndPutCarInGarage(std::vector<std::shared_ptr<car>>&,int);
	double getAverWeight();//获取道路的平均权重

	// 所有的车都是Finish状态 说明调度完成
	bool allCarFinish(std::vector<std::shared_ptr<car>>&);

	 bool isConflict(std::shared_ptr<car>);//是否发生冲突
	 void readAnswer(std::ifstream& in, std::vector<std::shared_ptr<car>>& car_list);
	 void saveAll(std::vector<std::shared_ptr<car>>&);
	 void recoverAll(std::vector<std::shared_ptr<car>>&); //重新规划后 恢复所有的路况信息
	 void updateWeight();
private:
	//std::map<Vertex<T>,std::list<Edge<K> *>> V//顶点集合
	//std::vector<std::vector<Edge<K>* >> E;//边集合
	//std::vector<Vertex> V;//由顶点指向外
	std::map<int, Vertex> v_map;//顶点的映射
	typedef std::pair<int, int> P; //first是最短距离，second是顶点的编号
	//std::vector<Vertex> VIN;//指向顶点
	int n, e;//顶点个数,边总数
};

void calculateTime(std::vector<std::shared_ptr<car>>&,int );