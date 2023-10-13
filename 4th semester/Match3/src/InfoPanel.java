import javax.swing.*;
import javax.swing.border.EtchedBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class InfoPanel extends JPanel {

    private final GamePanel parent;

    private long totalScore;
    private long n_o_moves;

    private final JButton veryImportantButton;
    private final JLabel scoreLabel;
    private final JLabel numOfMovesLabel;
    private final JLabel messageLabel;

    InfoPanel(GamePanel parent, int width, int height) {

        this.parent = parent;

        totalScore = 0;
        n_o_moves = 0;

        this.setBackground(Color.GRAY);
        this.setBorder(new EtchedBorder());
        this.setPreferredSize(new Dimension(width, height));

        this.setLayout(new GridLayout(4, 1));

        scoreLabel = new JLabel("Score: " + totalScore, SwingConstants.LEFT);
        this.add(scoreLabel);
        numOfMovesLabel = new JLabel("NOM: " + n_o_moves, SwingConstants.LEFT);
        this.add(numOfMovesLabel);
        messageLabel = new JLabel("Welcome!", SwingConstants.LEFT);
        this.add(messageLabel);
        veryImportantButton = new JButton("EXIT");
        veryImportantButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parent.getInteractivePanel().SendPoisonSignal();
                parent.getParent().getScorePanel().ChangeScores(totalScore, n_o_moves);
                parent.End();
                parent.getParent().getMyFrame().dispose();
            }
        });
        this.add(veryImportantButton);
    }

    public void ChangeScore(long score) {
        totalScore = score;
        scoreLabel.setText("Score: " + totalScore);
    }

    void IncrementNumOfMoves() {
        n_o_moves += 1;
        numOfMovesLabel.setText("NOM: " + n_o_moves);
    }

    void SetMessage(String message) {
        messageLabel.setText(message);
    }

}
