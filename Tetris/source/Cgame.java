


//对GridIn进行整理,加工;控制游戏的开始,结束,得分
public class Cgame{
	
	boolean[][] position;//存放放块位置的相对坐标
	boolean[][] position2;//提示框方块坐标
	int[][][] init_selectx={//初始化方块
	    	{{4,5,5,6},  {5,5,6,5},  {6,5,5,4},  {5,5,4,5}},  //1-T
	    	{{4,5,5,6},  {5,5,6,6},  {6,5,5,4},  {6,6,5,5}},  //1-Z
	    	{{4,5,5,6},  {6,6,5,5},  {6,5,5,4},  {5,5,6,6}},  //1-Z'
	    	{{5,5,5,6},  {4,5,6,6},  {6,6,6,5},  {6,5,4,4}},  //1-L
	    	{{4,5,5,5},  {6,6,5,4},  {6,5,5,5},  {4,4,5,6}},  //1-L'
	    	{{5,5,5,5},  {4,5,6,7},  {5,5,5,5},  {7,6,5,4}},  //1-I  
	    	{{5,5,6,6},  {5,6,6,5},  {6,6,5,5},  {6,5,5,4}}   //1-O
	    };
	    
    int[][][] init_selecty={
	    	{{3,3,4,3},  {4,3,3,2},  {4,4,3,4},  {2,3,3,4}},  //1-T
	    	{{3,3,4,4},  {4,3,3,2},  {4,4,3,3},  {2,3,3,4}},  //1-Z
	    	{{4,4,3,3},  {4,3,3,2},  {3,3,4,4},  {2,3,3,4}},  //1-Z'
	    	{{2,3,4,4},  {4,4,4,3},  {4,3,2,2},  {3,3,3,4}},  //1-L
	    	{{4,4,3,2},  {4,3,3,3},  {2,2,3,4},  {3,4,4,4}},  //1-L'
	    	{{1,2,3,4},  {4,4,4,4},  {4,3,2,1},  {4,4,4,4}},  //1-I
	    	{{3,4,4,3},  {4,4,3,3},  {4,3,3,4},  {3,3,4,4}}   //1-O
	    };
	
	public Cgame(){
		
		position=new boolean[24][10];
		position2=new boolean[4][4];
	}
	
	public void startGame(GridIn grid){};//开始游戏
	public void endGame(GridIn grid){};//结束游戏
	
	public boolean isGameFail(){//如果游戏失败,返回true
		
		boolean game=false;
		
		for(int j=0;j<10;j++){
			if(position[4][j]==true){
				game=true;
			}
		}
		
		return game;
	}
	
	public void setFullLine(Linefull line){//每次循环调用一次,设置满行line的属性
	
	    int flag;//记录每行小方块个数
	    
	    for(int i=0;i<24;i++){
	    	flag=0;       
	    	for(int j=0;j<10;j++){
	    		if(position[i][j]==true){
	    			flag++;
	    		}
	    		
	    		if(flag>=10){//该行为满行
	    			line.setFullRow(i);
	    			line.full=0;
	    			for(int t=0;t<24;t++){
	    				if(line.lineFullRow[t]==true){
	    				line.full=line.full+1;//加分	
	    				}
	    			}
	    		}
	    	}
	    }
	}
	
	public boolean freshGame(Linefull line){//删除满行,刷新界面
		    boolean boo=false;
			if(line.lineFullRow[0]==true){//若第一行为满行
				for(int p1=0;p1<10;p1++){
					position[0][p1]=false;
				}
				
				boo=true;
			}
			else{//满行不是第一行
			    for(int t=1;t<24;t++){
			    	if(line.lineFullRow[t]==true){//若第(t+1)行为满行
			    	    for(int at=t;at>=1;at--){//删除第(t+1)行
			    	    	for(int ai=0;ai<10;ai++){
			    	    		position[at][ai]=position[at-1][ai];
			    	    	}
			    	    }
			    	    
			    	    boo=true;
			    	}
			    }
			}
			
			return boo;	
	}
	
	public GridIn selectGrid(){//随机选取对象并放到position[][]中
	    	    	    
	    GridIn grid=new GridIn();
 	    grid.grid_type=(int)(Math.random()*7)+1; //取7种形式中的一种	  
  	    grid.grid_angle=(int)(Math.random()*4)+1; //取4种形式中的一种    	    	   
	    return grid;
	}
	
	public void reset_position(GridIn grid){
		for(int i=0;i<4;i++){
	   	    grid.grid_x[i]=init_selectx[grid.grid_type-1][grid.grid_angle-1][i];
	    	grid.grid_y[i]=init_selecty[grid.grid_type-1][grid.grid_angle-1][i];
	    	position[grid.grid_y[i]-1][grid.grid_x[i]-1]=true;
	    }
	}
	
	public void reset_position2(GridIn grid){
		
		for(int i1=0;i1<4;i1++){
	    	for(int j=0;j<4;j++){
	    		position2[i1][j]=false;
	    	}
	   	}	
	   	for(int i=0;i<4;i++){
	    	grid.grid_x[i]=init_selectx[grid.grid_type-1][grid.grid_angle-1][i];
	    	grid.grid_y[i]=init_selecty[grid.grid_type-1][grid.grid_angle-1][i]; 
	    	position2[grid.grid_y[i]-1][grid.grid_x[i]-4]=true;
	    }	
	}
	
}

