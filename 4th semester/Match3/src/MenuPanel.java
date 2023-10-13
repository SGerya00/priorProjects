import javax.swing.*;
import javax.swing.border.EtchedBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class MenuPanel extends JPanel {

    private final MainPanel parent;
    private CardLayout parentLayout;

    MenuPanel(MainPanel parent) {
        this.setBackground(Color.GRAY);
        this.setBorder(new EtchedBorder());
        this.setPreferredSize(new Dimension(600, 500));
        this.parent = parent;

        this.setLayout(new GridLayout(6, 1));

        JLabel title = new JLabel("Scuffed match-3 game");
        title.setHorizontalAlignment(SwingConstants.CENTER);
        Font font = new Font("SimSun-ExtB", Font.BOLD, 36);
        title.setFont(font);
        this.add(title);

        JButton button1 = new JButton("LEVEL 1: basic");
        button1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parent.getGamePanel().setConstraint(0);
                parentLayout.show(parent, "GamePanel");
                parent.getGamePanel().Start();
            }
        });
        this.add(button1);

        JButton button2 = new JButton("LEVEL 2: donut");
        button2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parent.getGamePanel().setConstraint(1);
                parentLayout.show(parent, "GamePanel");
                parent.getGamePanel().Start();
            }
        });
        this.add(button2);

        JButton button3 = new JButton("LEVEL 3: impossible");
        button3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parent.getGamePanel().setConstraint(2);
                parentLayout.show(parent, "GamePanel");
                parent.getGamePanel().Start();
            }
        });
        this.add(button3);

        JButton button4 = new JButton("SCORES");
        button4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parentLayout.show(parent, "ScorePanel");
            }
        });
        this.add(button4);

        JButton buttonExit = new JButton("EXIT");
        buttonExit.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parent.getMyFrame().dispose();
            }
        });
        this.add(buttonExit);
    }

    public void SpecifyParentLayout(CardLayout cr) {
        parentLayout = cr;
    }

}
