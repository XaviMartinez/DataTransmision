
import java.io.Console;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;


public class EmissorXat {
    
    public static void main(String[] args) throws IOException {
        
        DatagramSocket dSocket = null;
        DatagramPacket dPacket = null;

        int nMessages = 0;
        boolean exit  = false;
        boolean getIP = false;
        boolean sndPv = false;
        
        String userName = "";
        String chatIP   = "230.0.0.1";
        
        int chatPort        = 5555;
        int chatPrivatePort = 5550;
        
        String dataInfo = "";
        String dataInfoAux = "";

        if(args.length != 1 && args.length != 3 && args.length != 5) {
            System.out.println("Numero de argumentos erroneo!");
            return;
        }
        
        userName = args[0];
        for(int i = 1; i < args.length; i++) {
            if(args[i].equals("-p")) chatPort = Integer.parseInt(args[++i]);
            if(args[i].equals("-g")) chatIP = args[++i]; 
        }
        
        dataInfo = "R" + "#" + userName;
        dPacket = new DatagramPacket(dataInfo.getBytes(),
                                     dataInfo.getBytes().length,
                                     InetAddress.getByName(chatIP),
                                     chatPort);
        
        dSocket = new DatagramSocket();
        dSocket.setSoTimeout(3000);
        dSocket.send(dPacket);

        Console console = System.console();
        System.out.println("[Has en el chat " + chatIP + ":" + chatPort + "]");
        
        while(!exit) {
            dataInfo = "";
            System.out.print("Missatge? ");
            String strMsg = console.readLine();
            
            if(strMsg.startsWith("#sortir")) {
                dataInfo = "S" + "#" + userName;
                exit = true;
            } else if(strMsg.startsWith("#obteip")) {
                dataInfo = "I" + "#" + strMsg.split(" ")[1];
                getIP = true;
            } else if(strMsg.startsWith("#privat")) {
                String[] strSplit = strMsg.split(" "); 
                if(strSplit.length > 1) {
                    dataInfo = "I" + "#" + strSplit[1];
                    dataInfoAux = "M" + "#" +  userName + "#";
                    
                    for(int i = 2; i < strSplit.length; i++)
                        dataInfoAux += strSplit[i] + " ";
                    
                    getIP = false;
                    sndPv = true;
                }
            } else {
                dataInfo = "M" + "#" + userName + "#" + strMsg;
                nMessages++;
            }
            
            dPacket.setData(dataInfo.getBytes());
            dPacket.setLength(dataInfo.getBytes().length);
            
            dPacket.setAddress(InetAddress.getByName(chatIP));
            dPacket.setPort(chatPort);
            
            dSocket.send(dPacket);
            
            if(getIP || sndPv) {
                byte[] buf = new byte[1024];
                DatagramPacket dp = new DatagramPacket(buf, 1024);
                
                try{
                    dSocket.receive(dp);
                    String IP = new String(dp.getData());

                    if(getIP) {
                        System.out.println(" -> IP: " + IP.split("#")[1]);
                        getIP = false;
                    }

                    if(sndPv) {
                        dPacket.setData(dataInfoAux.getBytes());
                        dPacket.setLength(dataInfoAux.getBytes().length);

                        dPacket.setAddress(dp.getAddress());
                        dPacket.setPort(dp.getPort());

                        dSocket.send(dPacket);
                        
                        sndPv = false;
                        nMessages++;
                    }
                } catch (SocketTimeoutException e) {
                    System.out.println("[Usuario no Disponible]");
                    sndPv = false; getIP = false;
                }
            }
        }
        
        System.out.println("[Has salido del chat. " + nMessages + " enviados]");
        dSocket.close();
    }
    
}
