#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>

#include <entrada_acciones_mensajes/ContadorAction.h>
#include <entrada_acciones_mensajes/ContadorGoal.h>
#include <entrada_acciones_mensajes/ContadorResult.h>
#include <entrada_acciones_mensajes/ContadorFeedback.h>

class Contador{
    protected:
	ros::NodeHandle _nh;
	actionlib::SimpleActionServer<entrada_acciones_mensajes::ContadorAction> _as;
	int _contador;
    public:
	Contador(): 
		_as(_nh, "/contador", 
			boost::bind(&Contador::cb, this, _1), 
			false),
		_contador(0)
	{
		_as.start();
		ROS_INFO("Servidor de la accion iniciado");
	}
    void cb(const entrada_acciones_mensajes::ContadorGoalConstPtr &goal){
        ROS_INFO("Goal received");
        int g =goal->objetivo;  
        ROS_INFO("Objetivo : %d",g);

        _contador=0;
        ros::Rate rate(1);
        bool success = false;
		bool preempted = false;

        while (ros::ok()) {
			_contador++;
			if (_as.isPreemptRequested()) {
				preempted = true;
				break;
			}
			if (_contador > 30) {
				break;
			}
			if (_contador > g) {
				success = true;
				break;
			}
			ROS_INFO("%d", _contador);
			entrada_acciones_mensajes::ContadorFeedback feedback;
			feedback.proporcion = 
				(double)_contador / (double)g;
			_as.publishFeedback(feedback);
			rate.sleep();
		}
        entrada_acciones_mensajes::ContadorResult result;
		result.contador = _contador;
		ROS_INFO("Send goal result to client");

		if (preempted) {
			ROS_INFO("Preempted");
			_as.setPreempted(result);
		}
		else if (success) {
			ROS_INFO("Success");
			_as.setSucceeded(result);
		}
		else {
			ROS_INFO("Aborted");
			_as.setAborted(result);
		}
    }
};

int main (int argc, char **argv) {

	ros::init(argc, argv, "nodo_acciones");
    ROS_INFO("SERVIDOR ACCION");
    Contador contador;
    ros::spin();

    return 0;
}