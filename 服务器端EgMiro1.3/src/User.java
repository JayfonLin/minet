import java.net.*;

public class User {
	Socket socket;
	int P2pPort;
	String name;
	int lifetime;
	
	public User(Socket s, String n, int p){
		socket = s;
		name = n;
		lifetime = 10;
		P2pPort = p;
	}
	
	public Socket getSocket(){
		return socket;
	}
	
	public int getP2pPort(){
		return P2pPort;
	}
	
	public String getName(){
		return name;
	}
	
	public User getUser(){
		return this;
	}
}
