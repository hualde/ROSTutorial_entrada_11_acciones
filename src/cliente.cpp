#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>

#include <entrada_acciones_mensajes/ContadorAction.h>
#include <entrada_acciones_mensajes/ContadorGoal.h>
#include <entrada_acciones_mensajes/ContadorResult.h>
#include <entrada_acciones_mensajes/ContadorFeedback.h>

class ContadorCliente{

    protected:
	ros::NodeHandle _nh;
	actionlib::SimpleActionClient<entrada_acciones_mensajes::ContadorAction> _ac;

	public:
	ContadorCliente():
		_ac("/contador", true)
	{
		ROS_INFO("Wait for the action server to start...");
		_ac.waitForServer();
		ROS_INFO("Server is now up.");
	}

    void sendGoal() {
		entrada_acciones_mensajes::ContadorGoal goal;
		goal.objetivo = 12;
		_ac.sendGoal(goal, 
			boost::bind(&ContadorCliente::doneCb, this, _1, _2),
			boost::bind(&ContadorCliente::activeCb, this),
			boost::bind(&ContadorCliente::feedbackCb, this, _1));
		ROS_INFO("Goal has been sent.");

		// Uncomment to cancel the goal after 2 seconds
		//ros::Duration(2.0).sleep();
		//_ac.cancelGoal();
	}

	void doneCb(const actionlib::SimpleClientGoalState& state,
			const entrada_acciones_mensajes::ContadorResultConstPtr &result) 
	{
		ROS_INFO("Finished in state: %s", 
			state.toString().c_str());
		ROS_INFO("Count result: %d", (int)result->contador);
	}

	void activeCb() {
		ROS_INFO("Goal just went active");
	}

	void feedbackCb(const entrada_acciones_mensajes::ContadorFeedbackConstPtr &feedback) 
	{
		ROS_INFO("Feedback received. Percentage: %lf", 
			feedback->proporcion);
	}


};

int main (int argc, char **argv) {
	ros::init(argc, argv, "count_until_client");
	ContadorCliente cliente;
	cliente.sendGoal();
	ros::spin();
}