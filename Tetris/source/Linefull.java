


//慢行类
public class Linefull{
	
	boolean[] lineFullRow;//记录满行所在的行数
	int full;//慢行的行数
	
	public Linefull(){
		
		full=0;
		lineFullRow=new boolean[24];
		for(int i=0;i<24;i++){
			lineFullRow[i]=false;
		}
	}
	
	public void setFullRow(int row){
		
		if(row>=0&&row<24){
			this.lineFullRow[row]=true;
		}
	}
	
	public void freshLine(){//刷新并初始化属性
	
	    full=0;
		for(int i=0;i<24;i++){
			lineFullRow[i]=false;
		}
	}
}