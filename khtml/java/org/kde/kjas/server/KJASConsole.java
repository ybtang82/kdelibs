package org.kde.kjas.server;

import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class KJASConsole
    extends Frame
{
    private TextArea txt;

    public KJASConsole()
    {
        super("Java Console");

        txt = new TextArea();
        txt.setEditable(false);

        Panel main = new Panel(new BorderLayout());
        Panel btns = new Panel(new BorderLayout());

        Button clear = new Button("Clear");
        Button save  = new Button("Save");
        Button close = new Button("Close");
        
        btns.add(clear, "West");
        btns.add(save, "Center");
        btns.add(close, "East");

        main.add(txt, "Center");
        main.add(btns, "South");
        
        add( main );

        clear.addActionListener
        (
            new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    txt.setText("");
                }
            }
        );

        save.addActionListener
        (
            new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    try
                    {
                        //dump the console's contents to a file
                        String all_text = txt.getText();
                        FileOutputStream output = new FileOutputStream( "/tmp/kjas.log", false );
                        output.write( all_text.getBytes() );
                    }
                    catch( IOException ex )
                    {
                        Main.kjas_debug( "could not save output stream" );
                    }
                }
            }
        );

        close.addActionListener
        (
            new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    setVisible(false);
                }
            }
        );

        addWindowListener
        (
            new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    setVisible(false);
                }
            }
        );

        setSize(300, 300);

        PrintStream st = new PrintStream( new KJASConsoleStream(txt) );
        System.setOut(st);
        System.setErr(st);
        
        System.out.println( "Java VM version: " +
                            System.getProperty("java.version") );
        System.out.println( "Java VM vendor:  " +
                            System.getProperty("java.vendor") );
    }
}

class KJASConsoleStream
    extends OutputStream
{
    TextArea txt;

    public KJASConsoleStream( TextArea _txt )
    {
        txt = _txt;
    }

    public void close() {}
    public void flush() {}
    public void write(byte[] b) {}
    public void write(int a) {}

    // Should be enough for the console
    public void write( byte[] bytes, int offset, int length )
    {
        try  // Just in case
        {
            String msg = new String( bytes, offset, length );
            synchronized( txt )
            {
                //get the caret position, and then get the new position
                int old_pos = txt.getCaretPosition();
                txt.append(msg);
                txt.setCaretPosition( old_pos + length );
            }
        }
        catch(Throwable t) {}
    }
}

