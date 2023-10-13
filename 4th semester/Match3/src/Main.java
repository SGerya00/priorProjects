public class Main {

    private static void CreateAndShowGUI() {
        MyFrame frame = new MyFrame();
    }

    public static void main(String[] args) {

        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                CreateAndShowGUI();
            }
        });
    }

}
