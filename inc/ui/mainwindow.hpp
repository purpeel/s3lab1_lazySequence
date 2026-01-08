#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <regex>
#include <algorithm>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QMetaType>
#include "LazySequence.hpp"


#include <iostream>
Q_DECLARE_METATYPE(SharedPtr<LazySequence<int>>)

QT_BEGIN_NAMESPACE
namespace Ui 
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_createBtn_clicked();
    void on_deleteBtn_clicked();
    void on_appendBtn_clicked();
    void on_memoiseBtn_clicked();
    void on_prependBtn_clicked();
    void on_insertAtBtn_clicked();
    void on_concatBtn_clicked();
    void on_skipBtn_clicked();
    void on_subSeqBtn_clicked();
    void on_mapBtn_clicked();
    void on_whereBtn_clicked();
private:
    Ui::MainWindow *ui;

    template <typename T>
    T getFromInput( const QString& input ) const;
    
    static auto fibonacciRule() {
        return []( const ArraySequence<int>& previous ) -> int {
                                        return previous[0] + previous[1];
                                                                };
    }

    static auto NaturalRule() {
        return []( const ArraySequence<int>& previous ) -> int {
                                                return previous[0] + 1;
                                                                };
    }

    static auto mapDouble() {
        return []( const int elem ) -> int {
                                      return elem * 2;
                                            };
    }
    static auto mapSign() {
        return []( const int elem ) -> int {
                                      return elem > 0 ? 1 : -1;
                                            };
    }

    static auto whereEven() {
        return []( const int elem ) -> bool {
                                    return elem % 2 == 0;
                                             };
    }

    static QString contentsStr( const SharedPtr<LazySequence<int>>& item ) { 
        QString res = "{";
        auto buff = item->getMaterialized();

        int limit = 15;
        int size = buff.getSize();

        for (int i = std::max(0, size - limit); i < size; i++) {
            if (i != 0) { res += ", "; }
            res += QString::fromStdString( std::format( "{}", buff[i] ));
        }
        res += "}"; 
        
        return res;
    }

    void addToList( const SharedPtr<LazySequence<int>>& seq );

    void addToList( SharedPtr<LazySequence<int>>& item );
    void updateUI();
    void showError( const std::exception &ex );
    
    enum Rule {
        None = 0,
        Fibonacci = 1, 
        Natural = 2
    };
};

#endif // MAINWINDOW_H
