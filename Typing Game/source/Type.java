import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

class Letter extends JPanel implements Runnable{
	String Caption="";
	Boolean isStop=true,isLost=false;
	int x=0,y=-20;

	static int LastCount=0;	
	//获得字母 
	public String getCaption(){
		return(Caption);
	}
	//设置字母
	public void setCaption(String Caption){
		this.Caption=Caption;
	}
			
	public void paintComponent(Graphics g){
		super.paintComponent(g);
		Graphics2D g2d=(Graphics2D)g;
		Font f= new Font("黑体",Font.BOLD,24);
		FontMetrics fm=getFontMetrics(f);
		g2d.setFont(f);
		x=(getWidth()-fm.stringWidth(Caption))/2;
		g2d.drawString(Caption,x,y);	
		if(!isLost){
			g2d.setColor(Color.green);	
			g2d.drawLine(0,200,getWidth(),200);	
		}
		
	}
	//清除
	public void Clear(){
		y=-20;

		repaint();
	}
	//开始
	public void play(int x){
		isStop=true;
		Thread t=new Thread(this);
		t.start();
		this.x=x;
		
	}
	//停止
	public void stop(){
		isStop=false;
	}
	public int getLastCount(){
		return LastCount;
	}
	public void setLastCount(int LastCount){
		this.LastCount=LastCount;
	}
	public Boolean getIsLast(){
		return isLost;
	}
	public int getIntX(){
		return x;
	}	
	public int getIntY(){
		return y;
	}
	public void reStart(){
		LastCount=0;
		isLost=false;
	}
	public void run(){
		while(isStop){
			try{
				if(y>200){
					isLost=true;
					repaint();
					Clear();
					LastCount++;
				}
				repaint();
				y++;
				Thread.sleep(50);
			}
			catch(Exception e){
			}				

		}
	}
}
class MyPanel extends JPanel implements Runnable{
	Letter Let1[];
	Boolean isStop=true;
	MyPanel(){

		
		setLayout(new GridLayout(1,8));
		Let1=new Letter[8];
		for(int i=0;i<Let1.length;i++){
			Let1[i]=new Letter();
			add(Let1[i]);			
		}
			
	}
	//获得字母
	public void getCaption(String[] Caption){
		for(int i=0;i<Let1.length;i++){
			Caption[i]=Let1[i].getCaption();		
		}		
	}
	//清除
	public void Clear(int i){
		Let1[i].Clear();
	}
	//开始
	public void Play(){
		isStop=true;
		for(int i=0;i<Let1.length;i++){
			if(Let1[i].getIntX()>0){
				Let1[i].play(0);					
			}		
		}		
		Thread t=new Thread(this);
		t.start();
		
	}
	//停止
	public void stop(){
		for(int i=0;i<Let1.length;i++){
			Let1[i].stop();	
		}
		isStop=false;		
	}
	public Boolean getIsStop(){
		return isStop;
	}
	public void reStart(){
		for(int i=0;i<Let1.length;i++){
			Let1[i].reStart();	
		}	
				
	}		
	public void run(){
		int i=0,j=0;
		while(isStop){
			try{
				i=(int)(8.0*Math.random());

				if(Let1[i].getIntY()<0){
					j=(int)((90-65+1)*Math.random()+65);
					Let1[i].setCaption(""+(char)j);
					Let1[i].play(0);
					Thread.sleep(500);					
				}
				if(Let1[0].getLastCount()>Let1.length){
					stop();
					for(i=0;i<Let1.length;i++){
						Clear(i);			
					}
					reStart();					
					JOptionPane.showMessageDialog(null,"请再接再厉!");
		
				}
			}
			catch(Exception e){
			}
		}
	}

}
class MyFrame extends JFrame implements ActionListener{
	JButton butStart;
	JButton butExit;
	JLabel lblCount;
	
	MyPanel p1;
	String str[];
	int Count=0;
	MyFrame(String Title){
		super(Title);
		
		
		Container c=getContentPane();
		
		str=new String[8];
		p1=new MyPanel();
		JPanel p2=new JPanel();
		
		butStart=new JButton("开始");
		butExit=new JButton("退出");
		
		lblCount=new JLabel("击中数:"+Count);
		
		butStart.addActionListener(this);
		butExit.addActionListener(this);
		

		p2.add(lblCount);
		p2.add(butStart);
		p2.add(butExit);
		
		c.setLayout(new BorderLayout());
		
		c.add(p1);
		c.add(p2,BorderLayout.SOUTH);
		MyKeyAdapter bAction=new MyKeyAdapter();
		addKeyListener(bAction);
		butStart.addKeyListener(bAction);
		butExit.addKeyListener(bAction);
					
		addWindowListener(new WindowAdapter(){
			public void windowClosing(WindowEvent e){
				System.exit(0);
			}
		});
	}
	private class MyKeyAdapter extends KeyAdapter{
		public void keyTyped(KeyEvent e) {
			if(p1.getIsStop()){
				p1.getCaption(str);
				for(int i=0;i<str.length;i++){
					String s=new String(""+e.getKeyChar());
					if(str[i].equals(s.toUpperCase())){
						p1.Clear(i);
						Count++;
						lblCount.setText("击中数:"+Count);
						return;		
					}					
				}				
			}
			
		}        
	} 	
	public void actionPerformed(ActionEvent e){
		if(e.getSource()==butStart){
			if(butStart.getText().equals("开始")){
				p1.Play();
				butStart.setText("暂停");
				Count=0;
				lblCount.setText("击中数:"+Count);
			}
			else{
				p1.stop();
				butStart.setText("开始");
			}
		}	
		else if(e.getSource()==butExit){
			System.exit(0);
		}	
		
	}

}
public class Type {
	public static void main(String args[]){
		MyFrame f=new MyFrame("测试");
		f.setSize(new Dimension(300,300));
		f.setResizable(false);
		f.setFocusable(true);
		f.setVisible(true);
		}
}                    