import javax.swing.*;
import java.awt.*;

public class MainPanel extends JPanel {

    private final MyFrame myFrame;

    private final MenuPanel mp;
    public MenuPanel getMenuPanel() {
        return mp;
    }

    private final GamePanel gp;
    public GamePanel getGamePanel() {
        return gp;
    }

    private final ScorePanel sp;
    public ScorePanel getScorePanel() {
        return sp;
    }

    MainPanel(MyFrame myFrame) {
        this.setBackground(Color.GRAY);
        this.setPreferredSize(new Dimension(600, 500));
        this.myFrame = myFrame;

        mp = new MenuPanel(this);
        gp = new GamePanel(this);
        sp = new ScorePanel(this);

        CardLayout cl = new CardLayout();
        this.setLayout(cl);
        mp.SpecifyParentLayout(cl);
        this.add("MenuPanel", mp);
        this.add("GamePanel", gp);
        sp.SpecifyParentLayout(cl);
        this.add("ScorePanel", sp);
    }

    public MyFrame getMyFrame() {
        return myFrame;
    }

}
