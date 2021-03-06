﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Lab5
{
    class Connect2
    {
        int result;
        ManualResetEvent manualResetEvent;
        string adr;

        public Connect2(string adr)
        {
            this.adr = adr;
        }

        public int doTask()
        {
            result = 0;
            manualResetEvent = new ManualResetEvent(false);
            StartConnection1();
            manualResetEvent.WaitOne();
            return result;
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
            Send(s, "GET / HTTP/1.1 \nHost: " + adr + " \nConnection: keep-alive \nCache-Control: max-age=0 \nUpgrade-Insecure-Requests: 1 \nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.89 Safari/537.36 \nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8 \nAccept-Encoding: gzip, deflate \nAccept-Language: ro,en-US;q=0.9,en;q=0.8 \nCookie: pll_language=ro; TiddlyWiki=chkRegExpSearch:\"false\" chkCaseSensitiveSearch:\"false\" chkIncrementalSearch:\"true\" chkAnimate:\"true\" chkSaveBackups:\"true\" chkAutoSave:\"false\" chkGenerateAnRssFeed:\"false\" chkSaveEmptyTemplate:\"false\" chkOpenInNewWindow:\"true\" chkToggleLinks:\"false\" chkHttpReadOnly:\"true\" chkForceMinorUpdate:\"false\" chkConfirmDelete:\"true\" chkInsertTabs:\"false\" chkUsePreForStorage:\"true\" chkDisplayInstrumentation:\"false\" txtBackupFolder:\"\" txtEditorFocus:\"text\" txtMainTab:\"Timeline\" txtMoreTab:\"moreTabAll\" txtMaxEditRows:\"30\" txtFileSystemCharSet:\"UTF-8\" txtTheme:\"\" txtUserName:\"YourName\"; arp_scroll_position=500");
            StateObject so2 = new StateObject { workSocket = s };
            s.BeginReceive(so2.buffer, 0, StateObject.BUFFER_SIZE, 0, Read_Callback, so2);
            System.Threading.Thread.Sleep(500);
            Console.WriteLine("Exit ReceiveConnection 1");
        }


        public void Read_Callback(IAsyncResult ar)
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
                    strContent = so.sb.ToString().Substring(index1 + 16, index2 - index1 - 17);
                    result = int.Parse(strContent);
                    Console.WriteLine(strContent);
                    manualResetEvent.Set();
                }
                s.Close();
            }

        }

        public void Send(Socket client, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            client.BeginSend(byteData, 0, byteData.Length, SocketFlags.None,
                new AsyncCallback(SendCallback), client);
        }

        public void SendCallback(IAsyncResult ar)
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
