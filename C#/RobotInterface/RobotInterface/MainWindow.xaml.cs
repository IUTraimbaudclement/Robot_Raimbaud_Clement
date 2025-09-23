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

namespace RobotInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;

        public MainWindow()
        {
            serialPort1 = new ExtendedSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.Open();

            InitializeComponent();
        }

        private void TextBoxEmission_TextChanged(object sender, TextChangedEventArgs e)
        {


        }

        private void ButtonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if(ButtonEnvoyer.Background == Brushes.RoyalBlue)
            {
                ButtonEnvoyer.Background = Brushes.Beige;
                return;
            }

            ButtonEnvoyer.Background = Brushes.RoyalBlue;


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
    }
}