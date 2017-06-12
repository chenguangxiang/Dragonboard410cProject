import java.awt.Container;  
import java.awt.FlowLayout;  
  
import javax.swing.ImageIcon;  
import javax.swing.JButton;  
import javax.swing.JFrame;  
import javax.swing.JLabel;  
import javax.swing.JPanel;  
import javax.swing.JPasswordField;  
import javax.swing.JTextField;  
  
public class BgTest extends JFrame{  
      
    public BgTest(){  
        init();  
    }  
  
    private void init() {  
          
        JLabel lbl1 = new JLabel("用户名：");  
        JLabel lbl2 = new JLabel("密　码：");  
        JTextField txt = new JTextField(10);  
        JPasswordField pasw = new JPasswordField(10);  
        pasw.setEchoChar('*');  
        JButton btn1 = new JButton("登录");  
        JButton btn2 = new JButton("取消");  
          
        ImageIcon img = new ImageIcon("backimage.jpg");  
        //要设置的背景图片  
        JLabel imgLabel = new JLabel(img);  
        //将背景图放在标签里。  
        this.getLayeredPane().add(imgLabel, new Integer(Integer.MIN_VALUE));  
        //将背景标签添加到jfram的LayeredPane面板里。  
        imgLabel.setBounds(0, 0, img.getIconWidth(), img.getIconHeight());  
        // 设置背景标签的位置  
        Container contain = this.getContentPane();  
        ((JPanel) contain).setOpaque(false);   
        // 将内容面板设为透明。将LayeredPane面板中的背景显示出来。  
          
        contain.setLayout(new FlowLayout());  
        contain.add(lbl1);  
        contain.add(txt);  
        contain.add(lbl2);  
        contain.add(pasw);  
        contain.add(btn1);  
        contain.add(btn2);  
          
        this.setTitle("背景图设置");  
        this.setSize(200, 200);//设置窗体大小  
        this.setLocation(600, 300);  
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);  
        this.setResizable(false);  
        this.setVisible(true);  
    }  
  
    public static void main(String[] args) {  
          
        new BgTest();   
    }  
}  