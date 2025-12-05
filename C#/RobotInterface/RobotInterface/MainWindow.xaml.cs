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
using static System.Runtime.InteropServices.JavaScript.JSType;

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
            /*
            List<byte> byteList = new List<byte>();

            for (int i = 0; i < 20; i++)
            {
                byteList.Add((byte) (2 * i));
            }

            byte[] array = Encoding.ASCII.GetBytes(TextBoxEmission.Text);
            UartEncodeAndSendMessage(0x0080, array.Length, array);
            */

            UartEncodeAndSendMessage((int) Action.TEXT, "TEST".Length, Encoding.UTF8.GetBytes("TEST"));

            UartEncodeAndSendMessage((int) Action.LED, 2, new byte[] { 1, 1 });
            UartEncodeAndSendMessage((int) Action.LED, 2, new byte[] { 2, 1 });
            UartEncodeAndSendMessage((int) Action.LED, 2, new byte[] { 3, 1 });

            UartEncodeAndSendMessage((int) Action.IR, 3, new byte[] { 15, 19, 87 });

            UartEncodeAndSendMessage((int) Action.VIT, 2, new byte[] { 15, 19 });

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
                    msgDecodedPayloadIndex = 0;
                    if(msgDecodedPayloadLength>0)
                        rcvState = StateReception.Payload;
                    else
                        rcvState = StateReception.CheckSum;
                    break;

                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex] = c;
                    msgDecodedPayloadIndex++;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;

                    break;

                case StateReception.CheckSum:
                    byte receivedChecksum = c;

                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

                    if(calculatedChecksum == receivedChecksum)
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    else          
                        TextBoxReception.Text += "Erreur de Trame: " + Encoding.UTF8.GetString(msgDecodedPayload, 0, msgDecodedPayload.Length);

                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }


        public enum Action
        {
            TEXT = 0x0080,
            LED = 0x0020,
            IR = 0x0030,
            VIT = 0x0040,
            STATE = 0x0050
        }

        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_VITE_GAUCHE = 8,
            STATE_TOURNE_VITE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_VITE_DROITE = 10,
            STATE_TOURNE_VITE_DROITE_EN_COURS = 11,
            STATE_TOURNE_SUR_PLACE = 12,
            STATE_TOURNE_SUR_PLACE_EN_COURS = 13,
            STATE_ARRET = 14,
            STATE_ARRET_EN_COURS = 15,
            STATE_RECULE = 16,
            STATE_RECULE_EN_COURS = 17,
            STATE_AVANCE_LENT = 18,
            STATE_AVANCE_LENT_EN_COURS = 19
        }


        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch((Action)msgFunction)
            {
                case Action.TEXT:
                    TextBoxReception.Text += "Message reçu: " + Encoding.UTF8.GetString(msgPayload, 0, msgPayload.Length) + "\r\n";
                    break;

                case Action.LED:
                    if (msgPayloadLength != 2)
                        return;
                    int led = msgPayload[0];
                    int state = msgPayload[1];

                    if(led >= 1 && led <= 3 && state >= 0 && state <= 1)
                    {
                        switch (state)
                        {
                            case 1:
                                switch (led)
                                {
                                    case 1: 
                                        Led1.IsChecked = true;
                                        break;
                                    case 2:
                                        Led2.IsChecked = true;
                                        break;
                                    case 3:
                                        Led3.IsChecked = true;
                                        break;
                                }
                                break;
                            case 2:
                                switch (led)
                                {
                                    case 1:
                                        Led1.IsChecked = false;
                                        break;
                                    case 2:
                                        Led2.IsChecked = false;
                                        break;
                                    case 3:
                                        Led3.IsChecked = false;
                                        break;
                                }
                                break;
                        }
                    }
                    break;

                case Action.IR:
                    if (msgPayloadLength != 3)
                        return;

                    int leftIR = msgPayload[0];
                    int centerIR = msgPayload[1];
                    int rightIR = msgPayload[2];

                    if(leftIR >= 0 && leftIR <= 99 && centerIR >= 0 && centerIR <= 99 && rightIR >= 0 && rightIR <= 99)
                    {
                        IRGauche.Text = "IR Gauche : " + leftIR + " cm";
                        IRCentre.Text = "IR Centre : " + centerIR + " cm";
                        IRDroite.Text = "IR Droite : " + rightIR + " cm";
                    }
                    break;

                case Action.VIT:
                    if (msgPayloadLength != 2)
                        return;

                    int leftVIT = msgPayload[0];
                    int rightVIT = msgPayload[1];

                    if (leftVIT >= -100 && leftVIT <= 100 && rightVIT >= -100 && rightVIT <= 100)
                    {
                        VITGauche.Text = "Vitesse Gauche : " + leftVIT + " %";
                        VITDroite.Text = "Vitesse Droite : " + rightVIT + " %";
                    }
                    break;

                case Action.STATE:
                    if (msgPayloadLength != 5)
                        return;

                    int getState = msgPayload[0];

                    long millis = msgPayload[1] << 24;
                    millis += msgPayload[2] << 16;
                    millis += msgPayload[3] << 8;
                    millis += msgPayload[4] << 0;

                    int instant = (((int)msgPayload[1]) << 24) + (((int)msgPayload[2]) << 16) + (((int)msgPayload[3]) << 8) + ((int)msgPayload[4]);

                    TextBoxReception.Text += "\nRobot␣State : " +
                    ((StateRobot) (msgPayload[0])).ToString() +
                    " - " + instant.ToString() + " ms";
                    break;

                    //TextBoxReception.Text += "STATE: " + getState + " | " + msgPayload[1] + " " + msgPayload[2] + " " + msgPayload[3] + " " + msgPayload[4] + "\r\n";
                    //TextBoxReception.Text += "STATE: " + getState + " | " + millis + "\r\n";

                    //break;

            }


        }


    }
}