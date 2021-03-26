using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Security.Cryptography.X509Certificates;
using System.Collections.Specialized;
using System.Collections;
using System.Runtime.InteropServices;

namespace Giftbox
{
    class giftbox
    {
        static void Main(string[] args)
        {
            string path = @"chocolates.txt"; // string path to read from file
            string solution = @"giftbox.txt"; // string path to write to file
            List<Item> chocolates = new List<Item>(); // creates list for storing items from Item class

            using (StreamReader sr = File.OpenText(path)) // opens text file for reading
            {
                string s; // store line of characters
                while((s = sr.ReadLine())  != null) // reads line of character while characters aren't null
                {
                    string[] elements = s.Split(','); // splits each string into substrings separated by comma

                    string name = Convert.ToString(elements[0]); // stores first substring as name
                    double cost = Convert.ToDouble(elements[1]); // converts second substring to double and stores as cost
                    double value = Convert.ToDouble(elements[2]); // converts third substring to double and stores as retail value

                    Item item = new Item(name, cost, value); // creates new instance for class item
                    chocolates.Add(item); // adds new object to list
                }
            }

            Console.WriteLine("Displaying all chocolates:");
            Console.WriteLine();

            // Output all chocolates on console window
            for (int i = 0; i < chocolates.Count; i++)
            {
                Console.WriteLine(chocolates[i]);
            }
            Console.WriteLine();

            int numItems = chocolates.Count; // stores total number of chocolates
            int solutionSpace = (int)Math.Pow(2, numItems) - 1; // calculates number of solutions for full enumeration

            // vector bool created to ouptput true and false instead of 0 and 1
            bool[] vector = buildVector(0);

            double Cap = 1.96; // max cost
            int minChoc = 14; // minimum num of chocolates that must be in the giftbox

            double totalVal = 0; // stores value of each chocolate combination
            double totalCost = 0; // stores cost of each chocolate combination
            int numChoc = 0; // stores number of chocolates for each iteration
            int totalChoc = 0; // stores number of chocolates for current best solution

            string itemList = ""; // stores name of chocolate combination
            string bestItem = ""; // names of choclates in optimal solution
            double CurrentBestAlternative = 0; // stores alternative number for best solution
            double currentBestVal = 0.00; // stores tottal value for best solution
            double currentBestCost = 0.00; // stores tottal cost for best solution
            double Profit;

            Console.WriteLine("Calculating optimal solution...");
            Console.WriteLine();

            for (int i = 1; i < solutionSpace; i++)
            {
                vector = buildVector(i); // vector alternative as bit array
                for (int j = 0; j < vector.Length; j++)
                {
                    if (vector[j] == true) // match items whethere true or false
                    {
                        // if true items added to item list with their sum of weight and value
                        itemList += chocolates[j].Name + ", ";
                        totalCost += chocolates[j].Cost;
                        totalVal += chocolates[j].Value;
                        numChoc++; // increment number of chocolates added to the box
                    }
                }

                if (totalVal > currentBestVal && totalCost <= Cap && numChoc >= minChoc) // checks each constraint
                {
                    CurrentBestAlternative = i; // stores current best alternative
                    currentBestVal = totalVal; // stores total best value of best iteration
                    currentBestCost = totalCost; // stores total best cost of best iteration
                    bestItem = itemList; // stores all chocolate names of best iteration
                    totalChoc = numChoc; // stores total number of chocolates of best iteration
                }

                // item components are reset before next iteration
                itemList = "";
                totalCost = 0;
                totalVal = 0;
                numChoc = 0;
            }

            // Calculate profit
            Profit = currentBestVal - currentBestCost;

            // creates instance of Item to store optimal solution 
            Item optimalSolution = new Item(bestItem, currentBestCost, currentBestVal);

            // best alternative written on file
            if (!File.Exists(solution)) // checks if file exists
            {
                using (StreamWriter sw = File.CreateText(solution)) // opens file for writing
                {
                    sw.Write("{0}: ", CurrentBestAlternative); // writes optimal alterantive number to stream
                    sw.WriteLine(optimalSolution); //writes optimal solution to stream
                    sw.WriteLine("Profit: £{0}", Profit); //writes optimal profit to stream
                }
            }

            // optimal solution written on screen
            Console.Write("{0}: ", CurrentBestAlternative);
            Console.WriteLine(optimalSolution);
            Console.WriteLine("Profit: £{0}", Profit);
            Console.WriteLine("Total chocolates: {0}", totalChoc);
            Console.WriteLine();
            Console.WriteLine("Optimal solution found and written in file {0}", solution);
            Console.WriteLine("Press return to exit...");
            Console.ReadLine();
        }

        // creates vector from the alternatives as an array of bools
        static bool[] buildVector(int alternative) 
        {
            BitArray ba = new BitArray(new int[] { alternative });
            bool[] bits = new bool[ba.Count];
            ba.CopyTo(bits, 0);

            return bits;
        }
    }


    class Item
    {
        public string Name { get; set; }
        public double Cost { get; set; }
        public double Value { get; set; }


        public Item(string name, double cost, double value)
        {
            Name = name;
            Cost = cost;
            Value = value;
        }

        public override string ToString()
        {
            return "Name: " + Name + "\nCost: £" + Cost + "\nRetail Price: £" + Value;
        }
    }
}
