using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort_NS;
using System.IO.Ports;
using System.Windows.Threading;
using System;

namespace RobotInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        string receivedText = "";
        DispatcherTimer timerAffichage;
        Robot robot;

        public MainWindow()
        {
            serialPort1 = new ExtendedSerialPort("COM7", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            InitializeComponent();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            robot = new Robot();
        }

        private void TimerAffichage_Tick(object? sender, EventArgs e)
        {

            if(robot.byteListReceived.Count() > 0)
            {
                while (robot.byteListReceived.Count() > 0)
                {
                    Byte data = robot.byteListReceived.Dequeue();
                    TextBoxReception.Text += "0x"+data.ToString("X2")+" ";    
                }
            }

        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            foreach (byte value in e.Data)
            {
                robot.byteListReceived.Enqueue(value);
            }
        }

        private void TextBoxEmission_TextChanged(object sender, TextChangedEventArgs e)
        {


        }

        private void ButtonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            //    if(ButtonEnvoyer.Background == Brushes.RoyalBlue)
            //    {
            //        ButtonEnvoyer.Background = Brushes.Beige;
            //        //return;
            //    }

            //    ButtonEnvoyer.Background = Brushes.RoyalBlue;

            serialPort1.WriteLine(TextBoxEmission.Text);
            TextBoxEmission.Clear();

        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                TextBoxEmission.Text = TextBoxEmission.Text.Substring(0, TextBoxEmission.Text.Length - 1);

                ButtonEnvoyer_Click(sender, new RoutedEventArgs());
                ButtonEnvoyer.Background = Brushes.Beige;
            }




        }

        private void ButtonClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxReception.Text = "";
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            List<byte> byteList = new List<byte>();
            for (int i = 0; i < 20; i++)
            {
                byteList.Add((byte) (2 * i));
            }

            serialPort1.Write(byteList.ToArray(), 0, byteList.Count());




        }


    }
}