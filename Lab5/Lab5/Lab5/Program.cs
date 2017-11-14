using System;
using System.Net;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lab5
{
    class Program
    {
        static void Main(string[] args)
        {
            //Impl1();
            //Console.ReadLine();

            //Impl2();
            //Console.ReadLine();

            Impl3Async();
            Console.ReadLine();
        }

        private static void Impl2()
        {
            Connect2 connect = new Connect2("www.cs.ubbcluj.ro");
            Connect2 connect2 = new Connect2("www.ubbcluj.ro");

            Task<int> task = new Task<int>(connect.doTask);
            Task<int> task2 = new Task<int>(connect2.doTask);

            task.Start();
            task2.Start();
            task.Wait();
            task2.Wait();

            int result = task.Result;
            int result2 = task.Result;

            Console.WriteLine("result " + result);
            Console.WriteLine("result2 " + result2);
            System.Threading.Thread.Sleep(2000);
        }

        private static void Impl1()
        {
            Connect1 connect = new Connect1("www.cs.ubbcluj.ro");
            Connect1 connect2 = new Connect1("www.ubbcluj.ro");

            IAsyncResult result = connect.StartConnection1();
            IAsyncResult result2 = connect2.StartConnection1();

            System.Threading.Thread.Sleep(3000);
            while (!result.IsCompleted || !result2.IsCompleted) { }
            Console.ReadLine();
        }

        static async Task DoSomething(String adr)
        {
            Connect3 connect = new Connect3(adr);
            Task<int> task = connect.doTask();
            int result = await task;
            Console.WriteLine("result " + result);

        }

        static async void Impl3Async()
        {

            await DoSomething("www.ubbcluj.ro");
            await DoSomething("www.cs.ubbcluj.ro");

        }

    }
}
