


//用卡片式布局创建小格子，提供基本单元
import java.awt.*;
import java.awt.event.*;

public class GridBlock extends Panel{
	
	Label blockLabel;
	Button blockButton;
	CardLayout card;
	
	public GridBlock(){
		card=new CardLayout();
		setLayout(card);
		blockLabel=new Label();
		blockButton=new Button();
		blockButton.setBackground(Color.PINK);//*****************
		blockLabel.setBackground(Color.orange);
		add(blockButton,"button");
		add(blockLabel,"label");
	}
	
	public void showBlockLabel(){
		card.show(this,"label");
	}
	
	public void showBlockButton(){
		card.show(this,"button");
	}
}