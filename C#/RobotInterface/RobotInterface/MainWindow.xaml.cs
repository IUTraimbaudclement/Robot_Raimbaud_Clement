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
                    //TextBoxReception.Text += "0x"+data.ToString("X2")+" ";    
                    DecodeMessage(data);
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
            byte[] array = Encoding.ASCII.GetBytes(TextBoxEmission.Text);
            UartEncodeAndSendMessage(0x0080, array.Length, array);

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

            byte[] array = Encoding.ASCII.GetBytes(TextBoxEmission.Text);
            UartEncodeAndSendMessage(0x0080, array.Length, array);
        }

        public void UartEncodeAndSendMessage(int msgFunction,
            int msgPayloadLength, byte[] msgPayload)
        {
            List<byte> send = new List<byte>();
            send.Add(0xFE);
            send.Add((byte) (msgFunction >> 8));
            send.Add((byte) (msgFunction >> 0));
            send.Add((byte) (msgPayloadLength >> 8));
            send.Add((byte) (msgPayloadLength >> 0));

            foreach (byte b in msgPayload)
            {
                send.Add(b);
            }

            send.Add(CalculateChecksum(msgFunction, msgPayloadLength, msgPayload));

            serialPort1.Write(send.ToArray(), 0, send.Count());
        }
        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {

            byte checksum = 0;

            checksum ^= 0xFE;
            checksum ^= (byte) (msgFunction >> 8);
            checksum ^= (byte) (msgFunction >> 0);
            checksum ^= (byte) (msgPayloadLength >> 8);
            checksum ^= (byte) (msgPayloadLength >> 0);

            foreach(byte b in msgPayload)
            {
                checksum ^= b;
            }

            return checksum;
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;
                    break;
                case StateReception.FunctionMSB:
                    msgDecodedFunction = c<<8;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodedFunction += c << 0;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:

                    msgDecodedPayloadLength += c << 0;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:

                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                    {
                        msgDecodedPayloadIndex = 0;
                        rcvState = StateReception.CheckSum;
                    }

                    break;

                case StateReception.CheckSum:
                    byte receivedChecksum = c;

                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    if(calculatedChecksum == receivedChecksum)
                        TextBoxReception.Text += Encoding.UTF8.GetString(msgDecodedPayload, 0, msgDecodedPayload.Length);
                    else
                    {
                        TextBoxReception.Text += "Erreur de Trame: ";
                        TextBoxReception.Text += Encoding.UTF8.GetString(msgDecodedPayload, 0, msgDecodedPayload.Length);
                    }


                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }


        public enum Action
        {
            Text = 0x0080,
            LED = 0x0020,

        }




    }
}