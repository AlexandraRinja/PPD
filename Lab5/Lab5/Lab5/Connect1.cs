using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Lab5
{
    class Connect1
    {
        string adr;

        public Connect1(string adr)
        {
            this.adr = adr;
        }

        public IAsyncResult StartConnection1()
        {
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            return s.BeginConnect(adr, 80, requestCallback: RecieveConnection1, state: s);
        }

        public void RecieveConnection1(IAsyncResult ar)
        {
            Console.WriteLine("Enter ReceiveConnection 1");
            Socket s = (Socket)ar.AsyncState;
            Send(s, "GET / HTTP/1.1 \nHost: " + adr + " \n");
            StateObject so2 = new StateObject { workSocket = s };
            s.BeginReceive(so2.buffer, 0, StateObject.BUFFER_SIZE, 0, Read_Callback, so2);
            System.Threading.Thread.Sleep(500);
            Console.WriteLine("Exit ReceiveConnection 1");
        }

        public static void Read_Callback(IAsyncResult ar)
        {
            Console.WriteLine("Read");
            System.Threading.Thread.Sleep(500);
            StateObject so = (StateObject)ar.AsyncState;
            Socket s = so.workSocket;
            int read = s.EndReceive(ar);
            if (read > 0)
            {
                so.sb.Append(Encoding.ASCII.GetString(so.buffer, 0, read));
                s.BeginReceive(so.buffer, 0, StateObject.BUFFER_SIZE, 0,
                                         new AsyncCallback(Read_Callback), so);
            }
            else
            {
                if (so.sb.Length > 1)
                {
                    //All of the data has been read, so displays it to the console
                    string strContent;
                    int index1 = so.sb.ToString().IndexOf("Content-Length:");
                    int index2 = so.sb.ToString().IndexOf("Connection:");
                    strContent = so.sb.ToString().Substring(index1, index2 - index1 - 1);
                    Console.WriteLine(strContent);
                }
                s.Close();

            }

        }

        public static void Send(Socket client, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            client.BeginSend(byteData, 0, byteData.Length, SocketFlags.None,
                new AsyncCallback(SendCallback), client);
        }

        public static void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                int bytesSent = client.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to server.", bytesSent);

            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
}
