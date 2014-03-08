import java.net.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.TimerTask;
import java.util.Timer;

public class TCPServer {
	//store online users
	static CopyOnWriteArrayList<User> onlineUserList = new CopyOnWriteArrayList<User>();
	//GaoYuwei:life reduce
	static TimerTask tt=new TimerTask()
	{
		public void run()
		{
			for(User user:onlineUserList)
			{
				user.lifetime -= 1;
				
			}
		}
	};
	public static void main(String argv[]) throws Exception{
		
		//create a thread pool of size 20 for efficiency
		ExecutorService threadPool = Executors.newFixedThreadPool(20);
		ServerSocket miroSocket = null;
		Socket minetSocket = null;
		int serverPort = 6789;
		Timer clock = new Timer();
		clock.scheduleAtFixedRate(tt, 1000, 1000);
		//store online users, think about it
		//ArrayList<User> onlineUserList = new ArrayList<User>();
		
		try{
			miroSocket = new ServerSocket(serverPort);
			System.out.println("welcome to miro...");
			while(true){
				minetSocket = miroSocket.accept();
				
				for(User user:onlineUserList)
				{
					if(user.lifetime==0)
						onlineUserList.remove(user);
				}
				//start a new thread to process each user's request
				ProcessThread pt = new ProcessThread(minetSocket, onlineUserList);
                Thread thread = new Thread(pt);   
				threadPool.execute(thread);
			}
		} catch (Exception exception){
			exception.printStackTrace();
		} finally {
			try{
				miroSocket.close();
				threadPool.shutdown();
			} catch (Exception exception){}
		}
	}
}
