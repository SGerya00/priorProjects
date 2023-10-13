import javax.swing.*;
import javax.swing.border.EtchedBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;

public class ScorePanel extends JPanel {

    private final MainPanel parent;
    private CardLayout parentLayout;

    private final int scoreLines = 5;
    private long[] scoreAndMovesTable;

    private final String filename = "SCORES.txt";

    ScorePanel(MainPanel parent) {
        this.setBackground(Color.GRAY);
        this.setBorder(new EtchedBorder());
        this.setPreferredSize(new Dimension(600, 500));
        this.parent = parent;

        this.setLayout(new GridLayout(1 + scoreLines + 1, 2));

        JLabel titleScore = new JLabel("SCORE");
        titleScore.setHorizontalAlignment(SwingConstants.CENTER);
        Font font = new Font("SimSun-ExtB", Font.BOLD, 36);
        titleScore.setFont(font);
        this.add(titleScore);

        JLabel titleNOM = new JLabel("#MOVES");
        titleNOM.setHorizontalAlignment(SwingConstants.CENTER);
        //Font font = new Font("SimSun-ExtB", Font.BOLD, 36);
        titleNOM.setFont(font);
        this.add(titleNOM);

        scoreAndMovesTable = new long[scoreLines * 2];
        ReadScores();
        AddScoresToSelf();

        JButton button = new JButton("RETURN");
        button.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                parentLayout.show(parent, "MenuPanel");
            }
        });
        this.add(button);

    }

    private void PerformActionWithScoresBackup() {
        for (int i = 0; i < scoreLines; i++) {
            scoreAndMovesTable[i * 2] = 0;
            scoreAndMovesTable[(i * 2) + 1] = 0;
        }
    }

    public void ChangeScores(long score, long n_o_moves) {
        String tmpFileName = "tmpSCORES.txt";

        try (FileReader fr = new FileReader(filename);
             BufferedReader br = new BufferedReader(fr);
             FileWriter fw = new FileWriter(tmpFileName);
             BufferedWriter bw = new BufferedWriter(fw);){

            boolean stop_flag = false;
            for (int i = 0; i < scoreLines; i++) {
                String line = br.readLine();
                String[] lines = line.split(" ");
                long local_score = Integer.parseInt(lines[0]);
                long local_n_o_moves = Integer.parseInt(lines[1]);
                if (!stop_flag) {
                    if (local_score > score) {
                        bw.write(line + "\n");
                    } else if (local_score == score) {
                        if (local_n_o_moves <= n_o_moves) {
                            bw.write(line + "\n");
                        }
                    } else {
                        bw.write(score + " " + n_o_moves + "\n");
                        bw.write(line + "\n");
                        stop_flag = true;
                        i += 1;
                    }
                } else {
                    bw.write(line + "\n");
                }
            }
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }
        // Once everything is complete, delete old file..
        File oldFile = new File(filename);
        oldFile.delete();

        // And rename tmp file's name to old file name
        File newFile = new File(tmpFileName);
        newFile.renameTo(oldFile);
    }

    private void ReadScores() {
        try (FileReader fr = new FileReader(filename);
             BufferedReader br = new BufferedReader(fr)){

            for (int i = 0; i < scoreLines; i++) {
                String line = br.readLine();
                String[] lines = line.split(" ");
                long local_score = Integer.parseInt(lines[0]);
                long local_n_o_moves = Integer.parseInt(lines[1]);
                scoreAndMovesTable[i * 2] = local_score;
                scoreAndMovesTable[(i * 2) + 1] = local_n_o_moves;
            }
        } catch (Exception e) {
            System.err.println(e.getMessage());
            PerformActionWithScoresBackup();
        }
    }

    private void AddScoresToSelf() {
        for (int i = 0; i < scoreLines; i++) {
            JLabel score = new JLabel("" + scoreAndMovesTable[i * 2]);
            score.setHorizontalAlignment(SwingConstants.CENTER);
            Font font = new Font("SimSun-ExtB", Font.BOLD, 36);
            score.setFont(font);
            this.add(score);

            JLabel titleNOM = new JLabel("" + scoreAndMovesTable[(i * 2) + 1]);
            titleNOM.setHorizontalAlignment(SwingConstants.CENTER);
            titleNOM.setFont(font);
            this.add(titleNOM);
        }
    }

    public void SpecifyParentLayout(CardLayout cr) {
        parentLayout = cr;
    }

}
