// contest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "PathGraph.h"

void res_car(std::ifstream& in ,std::vector<std::shared_ptr<car>>& car_list)
{
	std::string read_data;
	int car_id;//汽车Id
	int from;//从哪个路口出发
	int to;//到哪个路口停止
	int speed;//汽车行驶最大速度
	int plantime;//最小出发时间
	int priority;//是否优先
	int preset;//是否为预置车辆
	while (std::getline(in, read_data))
	{
		if (read_data.find('#') == -1)
		{
			size_t a = read_data.find('(');
			std::string data = read_data.substr(a + 1,read_data.size()-2);//去掉（）
			std::replace(data.begin(), data.end(), ',', ' ');//用空格代替,
			std::istringstream iss(data);//字符流
			std::string item;
			iss >> item;
			car_id = std::stoi(item);

			iss >> item;
			from = std::stoi(item);

			iss >> item;
			to = std::stoi(item);

			iss >> item;
			speed = std::stoi(item);

			iss >> item;
			plantime = std::stoi(item);	
			
			iss >> item;
			priority = std::stoi(item);

			iss >> item;
			preset = std::stoi(item);

		//	std::cout << car_id << " " << from << " " << to << " " << speed << " " << plantime << std::endl;
			car_list.emplace_back(std::make_shared<car>(car_id, from, to, speed, plantime,priority,preset ));
			
		}
		
	}
}

void res_road(std::ifstream& in, std::vector<std::shared_ptr<road>>& road_list)
{
	std::string read_data;
	int road_id;//道路Id号
	int length;//道路长度
	int speed;//道路允许的最大速度
	int channel;//道路的车道数
	int from;//连接的路口七点
	int to;//连接的路口终点
	int isDuplex;//是否双向

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
			road_id = std::stoi(item);

			iss >> item;
			length = std::stoi(item);

			iss >> item;
			speed = std::stoi(item);

			iss >> item;
			channel = std::stoi(item);

			iss >> item;
			from = std::stoi(item);

			iss >> item;
			to = std::stoi(item);

			iss >> item;
			isDuplex = std::stoi(item);
			road_list.emplace_back(std::make_shared<road>(road_id, length, speed, channel, from, to, isDuplex));
		//std::cout << road_id << " " << length << " " << speed << " " << channel<< " " << from << to << " " << isDuplex <<  std::endl;
		}
	
	}
}

void res_cross(std::ifstream& in, std::vector<std::shared_ptr<cross>>& cross_list)
{
	std::string read_data;
	int cross_id;//路口Id
	int right_road_id;
	int buttom_road_id;
	int left_road_id;
	int top_road_id;
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
			cross_id = std::stoi(item);

			iss >> item;
			top_road_id = std::stoi(item);

			iss >> item;
			right_road_id = std::stoi(item);

			iss >> item;
			buttom_road_id = std::stoi(item);

			iss >> item;
			left_road_id = std::stoi(item);
			cross_list.emplace_back(std::make_shared<cross>(cross_id, top_road_id, right_road_id, buttom_road_id, left_road_id));
		}
		
	}
}




int main(int argc, char *argv[])
{
	std::cout << "Begin" << std::endl;

	if (argc < 6) {
		std::cout << "please input args: carPath, roadPath, crossPath, presetAnswerPath,answerPath" << std::endl;
		exit(1);
	}

	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string presetAnswerPath(argv[4]);
	std::string answerPath(argv[5]);                                                    
	

	PathGraph graph;

	std::ifstream car_in(carPath);//汽车文件输入流
	std::ifstream road_in(roadPath);//道路文件输入流
	std::ifstream cross_in(crossPath);//路口文件输入流
	std::ifstream answer_in(answerPath);//车辆文件输入流
	std::ifstream preCaranswer_in(presetAnswerPath);//预置车辆文件输入流
	std::vector<std::shared_ptr<car>> car_list;//还未开始行驶的车
	std::vector<std::shared_ptr<road>> road_list;
	std::vector<std::shared_ptr<cross>> cross_list;


	res_car(car_in,car_list);
	res_road(road_in, road_list);
	res_cross(cross_in, cross_list);

	graph.init(road_list, cross_list, answer_in,car_list);

	graph.readAnswer(answer_in, car_list);
	graph.readAnswer(preCaranswer_in, car_list);

	graph.start(car_list);
    
	std::cout << "End" << std::endl;

	return 0;
}
