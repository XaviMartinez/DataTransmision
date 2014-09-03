
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.Arrays;



public class ReceptorXat {
        
    public static void main(String[] args) throws IOException {
        
        MulticastSocket mSocket = null;
        DatagramSocket dSocket = null;
        DatagramPacket dPacket = null;
        
        String userName = "";
        String chatIP   = "230.0.0.1";
        
        int chatPort        = 5555;
        int chatPrivatePort = 5550;
        
        String dataInfo = "";
        byte[] byteInfo = new byte[1024];
        ReceptorPrivat receptorPrivat = null;
        
        if(args.length != 1 && args.length != 3 && args.length != 5 && args.length != 7) {
            System.out.println("Numero de argumentos erroneo!");
            return;
        }
        
        userName = args[0];
        for(int i = 1; i < args.length; i++) {
            if(args[i].equals("-p")) chatPort = Integer.parseInt(args[++i]);
            if(args[i].equals("-g")) chatIP = args[++i];
            if(args[i].equals("-ppr")) chatPrivatePort = Integer.parseInt(args[++i]);
        }
        
        mSocket = new MulticastSocket(chatPort);
        dSocket = new DatagramSocket(chatPrivatePort);
        mSocket.joinGroup(InetAddress.getByName(chatIP));
        
        receptorPrivat = new ReceptorPrivat(dSocket);
        receptorPrivat.start();
        
        while(true) {
            Arrays.fill(byteInfo, (byte)0);
            dPacket = new DatagramPacket(byteInfo, byteInfo.length);
            mSocket.receive(dPacket);
            
            dataInfo = new String(dPacket.getData());
            String[] dataSplit = dataInfo.split("#");
            
            if(dataSplit[0].equals("M")) {
                System.out.println(dataSplit[1] + " > " + dataSplit[2]);
            } else if(dataSplit[0].equals("S")) {
                System.out.println("[" + dataSplit[1] + " ha salido en el chat]");
            } else if(dataSplit[0].equals("R")) {
                System.out.println("[" + dataSplit[1] + " ha entrado en el chat]");
            } else if(dataSplit[0].equals("I") && dataSplit[1].trim().equals(userName)) {
                dataInfo = "P" + "#" + InetAddress.getLocalHost().getHostAddress();

                DatagramPacket dP = new DatagramPacket(dataInfo.getBytes(), 
                                                       dataInfo.getBytes().length,
                                                       dPacket.getAddress(),
                                                       dPacket.getPort());
                dSocket.send(dP);    
            }
        }
        
        //mSocket.leaveGroup(InetAddress.getByName(chatIP));
        //mSocket.close();  
    }
}

class ReceptorPrivat extends Thread {
    private DatagramSocket dSocket = null;
    
    public ReceptorPrivat(DatagramSocket dSocket) {
        super();
        this.dSocket = dSocket;
    }
    
    @Override public void run() {
        String dataInfo = null;
        byte[] byteInfo = new byte[1024];
        
        while(true) {
            try {
                DatagramPacket dPacket = new DatagramPacket(byteInfo, byteInfo.length);
                dSocket.receive(dPacket);
                
                dataInfo = new String(dPacket.getData());
                String[] dataSplit = dataInfo.split("#");
                
                if(dataSplit[0].equals("M")){
                    System.out.println(dataSplit[1] + " > " + dataSplit[2]);
                }
            } catch (IOException ex) {
                System.out.println("IOException en ReceptorPrivat");
            }
        }
    }
}
