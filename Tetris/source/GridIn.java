


//内部逻辑层GridIn类

public class GridIn{
	
	int[] grid_oldx;
	int[] grid_oldy;//记录方块对象的上一次坐标
	int[] grid_x;
	int[] grid_y;//当前方块对象坐标值，取相对坐标
	int grid_type;//方块类型7种:1-T,2-Z,3-Z',4-L,5-L',6-I,7-O
	int grid_angle;//方块旋转角度类型4种:1-0度,2-90度,3-180度,4-270度
	boolean can=true;//是否可操作,若是方块对象落到底部,则不可操作
	
	//******************************************************************
	public GridIn(){
		
		grid_oldx=new int[4];
		grid_oldy=new int[4];
		grid_x=new int[4];
		grid_y=new int[4];
		grid_type=1;
		grid_angle=1;
	}
	
	//*************************************************************************
	public boolean rotate(boolean[][] position){
		
		
		
		
		int init_rotatex[][][]={
			{{1,0,1,-1},  {1,0,-1,-1},  {-1,0,-1,1},  {-1,0,1,1}},  //1-T
			{{1,0,1,0},   {1,0,-1,-2},  {0,1,0,1},    {-2,-1,0,1}}, //2-Z	
			{{2,1,0,-1},  {0,-1,0,-1},  {-1,0,1,2},   {-1,0,-1,0}}, //3-Z'		    
			{{-1,0,1,0},  {1,0,-1,-2},  {1,0,-1,0},   {-1,0,1,2}},  //4-L
			{{2,1,0,-1},  {0,-1,0,1},   {-2,-1,0,1},  {0,1,0,-1}},  //5-L'
			{{-1,0,1,2},  {1,0,-1,-2},  {2,1,0,-1},   {-2,-1,0,1}}, //6-I
			{{0,1,0,-1},  {1,0,-1,0},   {0,-1,0,1},   {-1,0,1,0}}   //7-O
		};
		
		int init_rotatey[][][]={
			{{1,0,-1,-1}, {0,1,0,2},    {-2,-1,0,0},  {1,0,1,-1}},  //1-T
			{{1,0,-1,-2}, {0,1,0,1},    {-2,-1,0,1},  {1,0,1,0}},   //2-Z
			{{0,-1,0,-1}, {-1,0,1,2},   {-1,0,-1,0},  {2,1,0,-1}},  //3-Z'
			{{2,1,0,-1},  {0,-1,-2,-1}, {-1,0,1,2},   {-1,0,1,0}},  //4-L
			{{0,-1,0,1},  {-2,-1,0,1},  {1,2,1,0},    {1,0,-1,-2}}, //5-L' 
			{{3,2,1,0},   {0,-1,-2,-3}, {0,1,2,3},    {-3,-2,-1,0}},//6-I  
			{{1,0,-1,0},  {0,-1,0,1},   {-1,0,1,0},   {0,1,0,-1}}   //7-O 
		};
		
		int maxx,maxy,minx;
		int[] grid_tempx=new int[4];
		int[] grid_tempy=new int[4];;//中间坐标代理量
		
		for(int i=0;i<4;i++){//初始化要旋转以后的值
			grid_tempx[i]=this.grid_x[i]+
			  init_rotatex[this.grid_type-1][this.grid_angle-1][i];
			grid_tempy[i]=this.grid_y[i]+
			  init_rotatey[this.grid_type-1][this.grid_angle-1][i];
		}
		
		maxx=10;maxy=24;minx=1;
		
		//如果旋转后有格子的x坐标小于1或大于10,则相应右移或左移
		for(int i=0;i<4;i++){
			if(grid_tempx[i]>maxx){
				maxx=grid_tempx[i];
			}
		}
		
		for(int i=0;i<4;i++){
			grid_tempx[i]=grid_tempx[i]-(maxx-10);
		}
		
		for(int i=0;i<4;i++){
			if(grid_tempx[i]<minx){
				minx=grid_tempx[i];
			}
		}
		
		for(int i=0;i<4;i++){
			grid_tempx[i]=grid_tempx[i]-(minx-1);
		}
		
		//如果旋转后有格子坐标大于20,则相应上移
		for(int i=0;i<4;i++){
			if(grid_tempy[i]>maxy){
				maxy=grid_tempy[i];
			}
		}
		
		for(int i=0;i<4;i++){
			grid_tempy[i]=grid_tempy[i]-(maxy-24);
		}
		
		for(int i=0;i<4;i++){//如果旋转后有方块与其他方块位置冲突,则不予旋转
			if((position[grid_tempy[i]-1][grid_tempx[i]-1]==true)&&
			   ((grid_tempx[i]==grid_x[0])&&(grid_tempy[i]==grid_y[0])||
			    (grid_tempx[i]==grid_x[1])&&(grid_tempy[i]==grid_y[1])||
			    (grid_tempx[i]==grid_x[2])&&(grid_tempy[i]==grid_y[2])||
			    (grid_tempx[i]==grid_x[3])&&(grid_tempy[i]==grid_y[3]))
			    ==false){
			    	return false;
			    }
		}
		
		//修改方块参数
		for(int i=0;i<4;i++){
			grid_oldx[i]=grid_x[i];
			grid_oldy[i]=grid_y[i];
			grid_x[i]=grid_tempx[i];
			grid_y[i]=grid_tempy[i];
		}
		
		grid_angle=grid_angle%4+1;
		
		for(int i=0;i<4;i++){//方块旋转前的位置隐藏
			if(grid_oldx[i]>=1&&grid_oldx[i]<=10&&
			   grid_oldy[i]>=1&&grid_oldy[i]<=24){
			   	 position[grid_oldy[i]-1][grid_oldx[i]-1]=false;
			   }
		}
		
		for(int i=0;i<4;i++){
			if(grid_x[i]>=1&&grid_x[i]<=10&&
			   grid_y[i]>=1&&grid_y[i]<=24){
			   	 position[grid_y[i]-1][grid_x[i]-1]=true;
			   }
		}
		
		return true;
	}//rotate()方法
	
	
	//**********************************************************************
	public boolean moveLeft(boolean[][] position){//方块对象左移一格 
		
		int[] grid_tempx=new int[4];
		int[] grid_tempy=new int[4];//左移后的代理值
		
		for(int i=0;i<4;i++){
			grid_tempx[i]=grid_x[i]-1;
			grid_tempy[i]=grid_y[i];
		}
		
		for(int i=0;i<4;i++){//左移后若x坐标小于1,则不予处理
			if(grid_tempx[i]<1){
				return false;
			}
		}
		
		for(int i=0;i<4;i++){//如果左移后有方块与其他方块位置冲突,则不予旋转
			if((position[grid_tempy[i]-1][grid_tempx[i]-1]==true)&&
			   ((grid_tempx[i]==grid_x[0])&&(grid_tempy[i]==grid_y[0])||
			    (grid_tempx[i]==grid_x[1])&&(grid_tempy[i]==grid_y[1])||
			    (grid_tempx[i]==grid_x[2])&&(grid_tempy[i]==grid_y[2])||
			    (grid_tempx[i]==grid_x[3])&&(grid_tempy[i]==grid_y[3]))
			    ==false){
			    	return false;
			    }
		}
		
		//修改方块参数
		for(int i=0;i<4;i++){
			grid_oldx[i]=grid_x[i];
			grid_oldy[i]=grid_y[i];
			grid_x[i]=grid_tempx[i];
			grid_y[i]=grid_tempy[i];
		}	
		
		for(int i=0;i<4;i++){//方块左移前的位置隐藏
			if(grid_oldx[i]>=1&&grid_oldx[i]<=10&&
			   grid_oldy[i]>=1&&grid_oldy[i]<=24){
			   	 position[grid_oldy[i]-1][grid_oldx[i]-1]=false;
			   }
		}
		
		for(int i=0;i<4;i++){
			if(grid_x[i]>=1&&grid_x[i]<=10&&
			   grid_y[i]>=1&&grid_y[i]<=24){
			   	 position[grid_y[i]-1][grid_x[i]-1]=true;
			   }
		}
		
		return true;
	}//moveLeft()方法
	
	//**********************************************************************
	public boolean moveRight(boolean[][] position){//方块对象右移一格 
		
		int[] grid_tempx=new int[4];
		int[] grid_tempy=new int[4];//右移后的代理值
		
		for(int i=0;i<4;i++){
			grid_tempx[i]=grid_x[i]+1;
			grid_tempy[i]=grid_y[i];
		}
		
		for(int i=0;i<4;i++){//右移后若x坐标大于10,则不予处理
			if(grid_tempx[i]>10){
				return false;
			}
		}
		
		for(int i=0;i<4;i++){//如果右移后有方块与其他方块位置冲突,则不予旋转
			if((position[grid_tempy[i]-1][grid_tempx[i]-1]==true)&&
			   ((grid_tempx[i]==grid_x[0])&&(grid_tempy[i]==grid_y[0])||
			    (grid_tempx[i]==grid_x[1])&&(grid_tempy[i]==grid_y[1])||
			    (grid_tempx[i]==grid_x[2])&&(grid_tempy[i]==grid_y[2])||
			    (grid_tempx[i]==grid_x[3])&&(grid_tempy[i]==grid_y[3]))
			    ==false){
			    	return false;
			    }
		}
		
		//修改方块参数
		for(int i=0;i<4;i++){
			grid_oldx[i]=grid_x[i];
			grid_oldy[i]=grid_y[i];
			grid_x[i]=grid_tempx[i];
			grid_y[i]=grid_tempy[i];
		}	
		
		for(int i=0;i<4;i++){//方块右移前的位置隐藏
			if(grid_oldx[i]>=1&&grid_oldx[i]<=10&&
			   grid_oldy[i]>=1&&grid_oldy[i]<=24){
			   	 position[grid_oldy[i]-1][grid_oldx[i]-1]=false;
			   }
		}
		
		for(int i=0;i<4;i++){
			if(grid_x[i]>=1&&grid_x[i]<=10&&
			   grid_y[i]>=1&&grid_y[i]<=24){
			   	 position[grid_y[i]-1][grid_x[i]-1]=true;
			   }
		}
		
		return true;
	}//moveLeft()方法
	
	//*****************************************************************
	public boolean moveDown(boolean[][] position){
		
		int[] grid_tempx=new int[4];
		int[] grid_tempy=new int[4];//下移后的代理值
		
		for(int i=0;i<4;i++){
			grid_tempx[i]=grid_x[i];
			grid_tempy[i]=grid_y[i]+1;
		}
		
		for(int i=0;i<4;i++){//下移后若x坐标小于1,则不予处理
			if(grid_tempy[i]>24){
				return false;
			}
		}
		
		for(int i=0;i<4;i++){//如果下移后有方块与其他方块位置冲突,则不予旋转
			if((position[grid_tempy[i]-1][grid_tempx[i]-1]==true)&&
			   ((grid_tempx[i]==grid_x[0])&&(grid_tempy[i]==grid_y[0])||
			    (grid_tempx[i]==grid_x[1])&&(grid_tempy[i]==grid_y[1])||
			    (grid_tempx[i]==grid_x[2])&&(grid_tempy[i]==grid_y[2])||
			    (grid_tempx[i]==grid_x[3])&&(grid_tempy[i]==grid_y[3]))
			    ==false){
			    	return false;
			    }
		}
		
		//修改方块参数
		for(int i=0;i<4;i++){
			grid_oldx[i]=grid_x[i];
			grid_oldy[i]=grid_y[i];
			grid_x[i]=grid_tempx[i];
			grid_y[i]=grid_tempy[i];
		}	
		
		for(int i=0;i<4;i++){//方块下移前的位置隐藏
			if(grid_oldx[i]>=1&&grid_oldx[i]<=10&&
			   grid_oldy[i]>=1&&grid_oldy[i]<=24){
			   	 position[grid_oldy[i]-1][grid_oldx[i]-1]=false;
			   }
		}
		
		for(int i=0;i<4;i++){
			
			if(grid_x[i]>=1&&grid_x[i]<=10&&
			   grid_y[i]>=1&&grid_y[i]<=24){
			   	 position[grid_y[i]-1][grid_x[i]-1]=true;
			}
			  
		}
		
		return true;
	}//moveDown()方法
}