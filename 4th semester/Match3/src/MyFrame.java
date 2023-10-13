import javax.swing.*;
import java.awt.*;

public class MyFrame extends JFrame {

    MainPanel mp;

    MyFrame() {
        this.setLocationRelativeTo(null);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setResizable(false);

        this.getContentPane().setLayout(new BoxLayout(this.getContentPane(), BoxLayout.X_AXIS));
        this.setBackground(Color.DARK_GRAY);

        mp = new MainPanel(this);
        this.getContentPane().add(mp);

        this.pack();

        this.setVisible(true);
    }
}