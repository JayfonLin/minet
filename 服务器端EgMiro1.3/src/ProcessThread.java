import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.concurrent.CopyOnWriteArrayList;


public class ProcessThread implements Runnable{
    final static String VERSION = "CS1.0";
    Socket minetSocket;
    CopyOnWriteArrayList<User> onlineUserList;
    
    /**
     * mark whether this user is online
     */
    boolean online;
    
    public ProcessThread(Socket minetSocket,
            CopyOnWriteArrayList<User> onlineUserList) {
        this.minetSocket = minetSocket;
        this.onlineUserList = onlineUserList;
        online = true;
    }

    @Override
    public void run() {
        try{
            //message come from client 
            BufferedReader clientInput = new BufferedReader(
                    new InputStreamReader(minetSocket.getInputStream()));
            //message send to client through printWriter 
            PrintWriter printWriter = new PrintWriter(
                    minetSocket.getOutputStream(), true);
            while (online){
                String command;
                if ((command = clientInput.readLine()) != null){
                    String[] info = command.split(" ");
                    if (info.length == 2){
                        if (info[0].equalsIgnoreCase("MINET")){
                            handshake(printWriter, info);
                        }else if (info[1].equalsIgnoreCase("GETLIST")){
                            getList(clientInput, printWriter);
                        }
                    }
                    if (info.length == 3){
                        if (info[0].equalsIgnoreCase(VERSION)){
                            if (info[1].equalsIgnoreCase("LOGIN")){
                                login(clientInput, printWriter, info);
                            }else if (info[1].equalsIgnoreCase("LEAVE")){
                                String userName = info[2];
                                leave(clientInput, userName);
                            }else if (info[1].equalsIgnoreCase("MESSAGE")){
                                String userName = info[2];
                                sendMessage(clientInput, printWriter, userName);
                            }else if (info[1].equalsIgnoreCase("BEAT")){
                                beat(info[2]);
                            }
                        }
                    }
                }
            }
            clientInput.close();
            printWriter.close();
            closeSocket();
        }catch(Exception e){
            e.printStackTrace();
        }
    }
    public void closeSocket(){
        try {
            minetSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    private void handshake(PrintWriter printWriter, String[] info){
        try{
            if (info[1] != null){
                InetAddress ia = InetAddress.getLocalHost();
                String hostname = ia.getHostName();
                String response = "MIRO "+hostname;
                printWriter.println(response);
            }
            else{
                System.out.println("command is incomplete");
            }
        }catch(UnknownHostException e){
            e.printStackTrace(printWriter);
        }
    }
    private void login(BufferedReader br, PrintWriter pw, String[] info) 
            throws IOException{
        String line;
        int length = 0;
        int p2pPort = -1;
        while (!(line = br.readLine()).equals("")){
            String[] headline = line.split(" ");
            if (headline[0].equalsIgnoreCase("Content-Length")){
                length = Integer.valueOf(headline[1]);
            }
            if (headline[0].equalsIgnoreCase("Port")){
                p2pPort = Integer.valueOf(headline[1]);
            }
        }
        
        char[] d = new char[1024];
        br.read(d, 0, length);
        
        String requestLine = null;
        String userName = info[2];
        //System.out.println("user name: "+userName);
        String data = null;
        String headLine = null;
        boolean nameExist = false;
        for (int i = 0; i < onlineUserList.size(); i++){
            User u = onlineUserList.get(i);
            if (u.getName().equals(userName)){
                if (u.lifetime > 0){
                    nameExist = true;
                    requestLine =  VERSION+" STATUS 0\r\n";
                    if (u.getSocket().getInetAddress().getHostAddress().equals(
                            minetSocket.getInetAddress().getHostAddress())){
                        data = "Already logged in";
                    }else{
                        data = "Name has been used";
                    }
                    headLine = getDate()+getContentLength(data)+"\r\n";
                    String response = requestLine+headLine+data;
                    pw.print(response);
                    pw.flush();
                    break;
                }
            }
        }
        if (!nameExist){
            User u = new User(minetSocket, userName, p2pPort);
            onlineUserList.add(u);
            requestLine = VERSION+" STATUS 1\r\n";
            String date = getDate();
            headLine = date+getContentLength("")+"\r\n";
            String response = requestLine+headLine;
            pw.print(response);
            pw.flush();
            System.out.println("Send online list");
            sendOnLineList(pw);
            update(1, userName, minetSocket, p2pPort, date);
        }
    }
    
    private String getDate(){
        Locale systime = Locale.CHINA;
        SimpleDateFormat timeformat = new SimpleDateFormat(
                "yyyy-MM-dd HH:mm:ss",systime);
        String temptime = timeformat.format(new Date());
        String headDate = "Date " + temptime + "\r\n";
        return headDate;
    }

    private String getContentLength(String data){
        byte[] buff = data.getBytes();
        int length = buff.length;
        String contentLength = "Content-Length " + length + "\r\n";
        return contentLength;
    }
    
    private void sendOnLineList(PrintWriter pw){
        String requestLine = VERSION+" LIST\r\n";
        String data = "";
        //System.out.println("user count "+onlineUserList.size());
        for(int i = 0; i < onlineUserList.size(); i++){
            User user = onlineUserList.get(i);
            if (user.lifetime > 0){
                Socket socket = user.getSocket();
                data += (user.getName() + " " + socket.getInetAddress()
                        .getHostAddress() + " " + user.getP2pPort() + "\r\n");
            }   
        }
        data += ("\r\n");
        System.out.println("Online users: ");
        System.out.println(data);
        String headLine = getContentLength(data)+"\r\n";
        //String headLine = getDate()+getContentLength(data)+"\r\n";
        String response = requestLine+headLine+data;
        pw.print(response);
        pw.flush();
    }
    private void update(int status, String name, Socket socket, 
            int p2pPort, String date){
        String hintMsg="";
        if (status == 0){
            hintMsg = name+" off line";
        }else if (status == 1){
            hintMsg = name+" on line";
        }
        System.out.println(hintMsg);
        String requestLine = VERSION+" UPDATE "+status+" "+name+"\r\n";
        String headLine = date+"\r\n";
        String response = requestLine+headLine;
        for (int i = 0; i < onlineUserList.size(); i++){
            User u = onlineUserList.get(i);
            if (!u.getName().equals(name)){
                if (u.lifetime > 0){
                    try{
                        PrintWriter pw = new PrintWriter(
                                u.getSocket().getOutputStream(), true);
                        pw.print(response);
                        pw.flush();
                        pw = null;
                    }catch(IOException e){
                        e.printStackTrace();
                    }
                }
            }
        }
        
    }
    private void getList(BufferedReader br, PrintWriter pw)
            throws NumberFormatException, IOException{
        String line;
        int length = 0;
        while (!(line = br.readLine()).equals("")){
            String[] headLine = line.split(" ");
            if (headLine[0].equalsIgnoreCase("Content-Length")){
                length = Integer.valueOf(headLine[1]);
            }
        }
        char[] d = new char[length];
        br.read(d, 0, length);
        
        sendOnLineList(pw);
    }
    private void leave(BufferedReader br, String userName)
            throws IOException{
        String line;
        String date = null;
        while (!(line = br.readLine()).equals("")){
            String[] headLine = line.split(" ");
            if (headLine[0].equalsIgnoreCase("Date")){
                date = headLine+"\r\n";
            }
        }
        for (int i = 0; i < onlineUserList.size(); i++){
            User user = onlineUserList.get(i);
            if(user.getName().equals(userName)){
                onlineUserList.remove(i);
                online = false;
                update(0, userName, minetSocket, user.getP2pPort(), date);
                break;
            }
        }
    }
    private void sendMessage(BufferedReader br, PrintWriter pw,
            String userName) throws IOException{
        String line;
        String date = "";
        int length = 0;
        while (!(line = br.readLine()).equals("")){
            String[] headLine = line.split(" ");
            if (headLine[0].equalsIgnoreCase("Content-Length")){
                length = Integer.valueOf(headLine[1]);
            }else if (headLine[0].equalsIgnoreCase("Date")){
                date = line+"\r\n";
            }
        }
        char[] msg = new char[length];
        br.read(msg, 0, length);
        String data = String.valueOf(msg);
        String requestLine = VERSION+" CSMESSAGE "+userName+"\r\n";
        //String headLine = date+getContentLength(data)+"\r\n";
        String headLine = getContentLength(data)+"\r\n";
        String response = requestLine+headLine+data;
        System.out.println("message send by "+userName+":");
        System.out.println(data);
        for (int i = 0; i < onlineUserList.size(); i++){
            User u = onlineUserList.get(i);
            if (u.lifetime > 0){
                try{
                    if (!userName.equals(u.getName())){
                        PrintWriter printWriter = new PrintWriter(
                                u.getSocket().getOutputStream(), true);
                        printWriter.print(response);
                        printWriter.flush();
                        printWriter = null;
                    }else{
                        pw.print(response);
                        pw.flush();
                    }
                }catch(IOException e){
                    e.printStackTrace();
                }
            }
        }
    }
    private void beat(String name) 
            throws NumberFormatException, IOException{
        for(User user:onlineUserList)
        {
            if(user.name.equals(name))
                user.lifetime = 10;
        }
    }
    
}
