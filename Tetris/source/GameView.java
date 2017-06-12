


//用户界面 GameView
import java.awt.*;
import java.applet.*;
import java.awt.event.*;

public class GameView extends Applet implements KeyListener,Runnable{
	
	Panel panel;
	Panel panel2;
	GridIn gridIn;//内部方块逻辑类
	GridIn gridIn2=null;//内部方块逻辑类,提示类
	Cgame cgame;//管理界面类
	Linefull line;//满行类
	GridBlock[][] gridBlock;//20*10平铺的卡片布局小方格数组
	GridBlock[][] gridBlock2;//4*4平铺的卡片布局小方格数组,提示框
	Thread thread;//线程,控制游戏进度
	int delay=500;//控制线程停顿时间
	int su=500;//控制游戏速度
	int fang=0;//方向标志
	int gameInt=1;//记录游戏等级
	String gameMessage="祝您成功！";	
	boolean diFlag=false;//如果方块到达底部时被移动了,
	               //此方块还能下落,则不转移对象,diFlag=true;
	int count=0;//记录分数
	int sp=1;//记录等级的助标记
	
	Label message1;//显示当前游戏等级****************************
	Label message2;//显示当前信息
	TextField text;//显示分数
	boolean stop=false;//方块对象到达底部时返true
	int m=0;
	Button button;
	
	int mm=0;//声音标志
	private AudioClip sound1;
	private AudioClip sound2;
    private AudioClip sound3;
    private AudioClip sound4;
	
	public void init(){
		
		sound1=getAudioClip(getDocumentBase(),"声音文件/SOUND8.WAV");	//旋转,左移与右移
		sound2=getAudioClip(getDocumentBase(),"声音文件/SOUND8.WAV");  //失败时
		sound3=getAudioClip(getDocumentBase(),"声音文件/SOUND104.WAV");//到达底部
		sound4=getAudioClip(getDocumentBase(),"声音文件/SOUND53.WAV"); //得分
		
		this.setLayout(null);
		cgame=new Cgame();  
		line=new Linefull();
		text=new TextField("得分:"+count);
		
		message1=new Label();
		this.add(message1);
		message1.setBounds(260,150,100,20);
		
		message2=new Label();
		this.add(message2);
		message2.setBounds(260,170,100,20);
		
		this.add(text);
		text.setBounds(50,10,80,22);
		text.setBackground(Color.YELLOW);

		panel=new Panel();	
		this.add(panel);
		panel.setBounds(50,50,200,400);
		panel.setLayout(new GridLayout(20,10,0,0));
		
		panel2=new Panel();	
		this.add(panel2);
		panel2.setBounds(260,50,80,80);
		panel2.setLayout(new GridLayout(4,4,0,0));
		
		//**************************************
	
		gridBlock=new GridBlock[20][10];
		
		for(int i=0;i<20;i++){
			for(int j=0;j<10;j++){
				gridBlock[i][j]=new GridBlock();
				panel.add(gridBlock[i][j]);
				gridBlock[i][j].showBlockLabel();
			}
		}
		
		gridBlock2=new GridBlock[4][4];//提示框
		
		for(int i=0;i<4;i++){
			for(int j=0;j<4;j++){
				gridBlock2[i][j]=new GridBlock();
				panel2.add(gridBlock2[i][j]);
				gridBlock2[i][j].showBlockLabel();
			}
		}
		
		button=new Button("重新开始");
		this.add(button);
		button.setBackground(Color.MAGENTA);
		button.setBounds(200,5,80,30);
		button.addActionListener(
			new ActionListener(){
				public void actionPerformed(ActionEvent event){
					initGame();
                }

			}
		);
		
		button.addKeyListener(this);
	    button.requestFocus();
		
	}
	
	public void initGame(){//调处对话框
	    for(int i=0;i<24;i++){
	         for(int j=0;j<10;j++){
   	             cgame.position[i][j]=false;
	         }
        }
	    gridIn=cgame.selectGrid();
	    cgame.reset_position(gridIn);
	    cgame.reset_position2(gridIn2);
	    count=0;
	  
	}
	
	public void soundStart(){
		
		if(mm==1){//旋转
			mm=0;
			sound1.play();
		}
		else if(mm==2){//左移与右移
			mm=0;
			sound2.play();
		}
		else if(mm==3){//到达底部
			mm=0;
			sound3.play();
		}
		else if(mm==4){//得分
			mm=0;
			sound4.play();
		}
		else{
			mm=0;
		}
		
	}
	
	public void start(){
		if(thread==null)	
		    thread=new Thread(this);	
		thread.start();
	}
	
	public void stop(){
		thread=null;//线程挂起
	}
	
	
	
	public void paint(Graphics g){
		button.requestFocus();
		g.drawRect(48,48,203,403);
		text.setText("得分:"+count*200);
		message1.setText("游戏等级为："+gameInt);
		message2.setText("游戏速度为"+(550-delay)/5);//****************************
	}
	
	public void run(){
		while(thread!=null){
			repaint();
			try{
				Thread.sleep(delay);
			}
			catch(InterruptedException e){}
			
			
			for(int i=0;i<20;i++){
				for(int j=0;j<10;j++){
					if(cgame.position[i+4][j]==true){
						gridBlock[i][j].showBlockButton();
					}
					else{
						gridBlock[i][j].showBlockLabel();
					}
				}
			}
			
	
			if(stop==false&&diFlag==false){
				
				count=count+line.full;//加分	
				if(gridIn2!=null){
					gridIn=gridIn2;
					
				}
				else{		
					gridIn=cgame.selectGrid();//得到新方块对象
					
				}
				gridIn2=cgame.selectGrid();//得到新方块对象
				cgame.reset_position(gridIn);
				cgame.reset_position2(gridIn2);
			}
			
	        line.freshLine();//刷新满行
			for(int i=0;i<4;i++){
				for(int j=0;j<4;j++){
					if(cgame.position2[i][j]==true){
						gridBlock2[i][j].showBlockButton();
					}
					else{
						gridBlock2[i][j].showBlockLabel();
					}
				}
			}
			
			stop=gridIn.moveDown(cgame.position);//向下移动成功，则返true
			cgame.setFullLine(line);//设置满行
			
			if(stop==false){//到达底部
				
				mm=3;//播放到达底部的音乐
				this.soundStart();
				
				if(cgame.freshGame(line)==true){//删除满行成功
				    mm=4;
				    this.soundStart();
				}
			    
			    diFlag=false;	
			}	
			    
			if(stop==false&&cgame.isGameFail()==true){
			    this.initGame();
			}  //如果游戏失败
	        
	        //此处控制游戏等级和游戏生机后的速度
	        if(count/25>sp){
	            sp=sp+1;
	        	count=count;
	        	if(su<=50){
	        		su=50;
	        	}
	        	else{
	        		su=su-50;
	        	}
	        	
	        	delay=su;
	        	gameInt=gameInt+1;	
	        }
	               
		}//while
		
	}//run
	
	public void keyPressed(KeyEvent e){
		
		mm=2;
		this.soundStart();	
		if(e.getKeyCode()==KeyEvent.VK_UP||e.getKeyCode()==KeyEvent.VK_W){
			//向上，则对象旋转
			gridIn.rotate(cgame.position);
		}
		else if(e.getKeyCode()==KeyEvent.VK_LEFT||e.getKeyCode()==KeyEvent.VK_A){
			//向左，则对象向左
			gridIn.moveLeft(cgame.position);
			
			if(stop==false){
			    diFlag=true;	
			}	
		}
		else if(e.getKeyCode()==KeyEvent.VK_RIGHT||e.getKeyCode()==KeyEvent.VK_D){
			//向右，则对象向右
		    gridIn.moveRight(cgame.position);
		    if(stop==false){
			    diFlag=true;	
			}	
		}
		else if(e.getKeyCode()==KeyEvent.VK_DOWN||e.getKeyCode()==KeyEvent.VK_S){
			//向下，则对象加速下落
		    delay=30;
		}
		else{}
	}
	  
	public void keyTyped(KeyEvent e){}
	public void keyReleased(KeyEvent e){
	    delay=su;
	    mm=0;
	}
}

