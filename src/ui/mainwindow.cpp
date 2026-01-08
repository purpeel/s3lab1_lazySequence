#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget* parent ) 
    : QMainWindow(parent)
    , ui( new Ui::MainWindow )
{
    ui->setupUi(this);
    ui->seqList->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showError( const std::exception& ex ) {
    QString message = QString( ex.what() );
    statusBar()->showMessage( message, 5000 );
}

void MainWindow::addToList( const SharedPtr<LazySequence<int>>& seq ) {
    QListWidgetItem* item = new QListWidgetItem( contentsStr(seq) );
    item->setData( Qt::UserRole, QVariant::fromValue(seq) );
    ui->seqList->addItem(item);
}

template <>
Ordinal MainWindow::getFromInput<Ordinal>( const QString& input ) const {
    std::regex r(R"(^(?:w(?:\*\d+)?(?:\+\d+)?|\d+)$)");
    std::string str = input.toStdString();
    if (!std::regex_match(str, r)) {
        std::cout << "im stupid" << std::endl;
        throw Exception( Exception::ErrorCode::INVALID_INPUT );
    } else {
        Ordinal res;
        if (str[0] == 'w') {
            std::string multiplier, tail;
            bool parsingMultiplier = false, parsingTail = false;
            for (size_t i = 0; i < str.length(); i++) {
                if (str[i] == '*') {
                    parsingMultiplier = true;
                    continue;
                } else if (str[i] == '+') {
                    parsingMultiplier = false;
                    parsingTail = true;
                    continue;
                }

                if (parsingMultiplier) {
                    multiplier += str[i];
                }
    
                if (parsingTail) {
                    tail += str[i];
                }
            }
            if (multiplier.empty()) {
                res = Ordinal::omega();
            } else {
                res = Ordinal::omega() * QString::fromStdString(multiplier).toULong();
            }   
            if (!tail.empty()) {
                res = res + QString::fromStdString(tail).toULong();
            }
        } else {
            res = Ordinal( input.toULong() );
        }
        return res;
    }
}

template <>
int MainWindow::getFromInput<int>( const QString& input ) const {
    std::regex r(R"([+-]?[0-9]+)");
    std::string str = input.toStdString();
    if (!std::regex_match(str, r)) {
        throw Exception(Exception::ErrorCode::INVALID_INPUT);
    }
    return input.toInt();
}

void MainWindow::on_createBtn_clicked()
{
    try {
        auto* item = new QListWidgetItem();
        if (ui->recurRule->currentIndex() == Rule::None) {
            item->setData( Qt::UserRole, QVariant::fromValue( LazySequence<int>::create() ));
        } else if (ui->recurRule->currentIndex() == Rule::Natural) {
            auto data = ArraySequence<int>();
            data.append(1);
            item->setData( Qt::UserRole, QVariant::fromValue(
                             LazySequence<int>::create(1, MainWindow::NaturalRule(), data) 
                                                             ));
        } else {
            auto data = ArraySequence<int>();
            data.append(0);
            data.append(1);
            item->setData( Qt::UserRole, QVariant::fromValue(
                             LazySequence<int>::create(2, MainWindow::fibonacciRule(), data) 
                                                             ));
        }
        item->setText( 
            contentsStr( item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>() )
                     );
                      
        ui->seqList->addItem(item);
    } catch ( std::exception& ex ) {
        showError( Exception(ex) );
    }
}

void MainWindow::on_deleteBtn_clicked() 
{
    try {
        auto *item = ui->seqList->takeItem(ui->seqList->currentRow());
        if (item) {
            delete item;
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_appendBtn_clicked()
{
    try {
        auto selected = ui->seqList->selectedItems();
        if (selected.size() == 1) {
            auto* item = selected[0];

            if (item) {
                auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
                if (ui->valueInput->text().isEmpty()) {
                    throw Exception( Exception::ErrorCode::INVALID_INPUT );
                }

                addToList( seq->append( getFromInput<int>( ui->valueInput->text() )));
            } else {
                throw Exception( Exception::ErrorCode::INVALID_SELECTION );
            }
        } else if (selected.size() == 2) {
            auto* item1 = selected[0];
            auto* item2 = selected[1];

            if (item1 && item2) {
                auto seq1 = item1->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
                auto seq2 = item2->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();

                addToList( seq1->append( *seq2 ) );
            } else {
                throw Exception( Exception::ErrorCode::INVALID_SELECTION );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_prependBtn_clicked()
{
    try {
        auto selected = ui->seqList->selectedItems();
        if (selected.size() == 1) {
            auto* item = selected[0];

            if (item) {
                auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
                if (ui->valueInput->text().isEmpty()) {
                    throw Exception( Exception::ErrorCode::INVALID_INPUT );
                }

                addToList( seq->prepend( getFromInput<int>( ui->valueInput->text() )));
            } else {
                throw Exception( Exception::ErrorCode::INVALID_SELECTION );
            }
        } else if (selected.size() == 2) {
            auto* item1 = selected[0];
            auto* item2 = selected[1];

            if (item1 && item2) {
                auto seq1 = item1->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
                auto seq2 = item2->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();

                addToList( seq1->prepend( *seq2 ) );
            } else {
                throw Exception( Exception::ErrorCode::INVALID_SELECTION );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_memoiseBtn_clicked() 
{
    try {
        auto* item = ui->seqList->item( ui->seqList->currentRow());
        if (item) {
            auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();

            seq->memoiseNext();

            item->setText( contentsStr(seq) );
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_insertAtBtn_clicked()
{
    try {
        auto selected = ui->seqList->selectedItems();
        if (selected.size() == 1) {
            auto* item = selected[0];

            if (item) {
                auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
                if (ui->valueInput->text().isEmpty() || ui->index1Input->text().isEmpty()) {
                    throw Exception( Exception::ErrorCode::INVALID_INPUT );
                }

                addToList( seq->insertAt( 
        getFromInput<int>( ui->valueInput->text()), getFromInput<Ordinal>( ui->index1Input->text()) 
                                         ));
            } else {
                throw Exception( Exception::ErrorCode::INVALID_SELECTION );
            }
        } else if (selected.size() == 2) {
            auto* item1 = selected[0];
            auto* item2 = selected[1];

            if (item1 && item2) {
                auto seq1 = item1->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
                auto seq2 = item2->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();

                addToList( seq1->insertAt( 
                    *seq2, getFromInput<Ordinal>( ui->index1Input->text()) 
                                         ));
            } else {
                throw Exception( Exception::ErrorCode::INVALID_SELECTION );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError(ex);
    }
}

void MainWindow::on_concatBtn_clicked() 
{
    try {
        auto selected = ui->seqList->selectedItems();
        if (selected.size() != 2) {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }

        auto* item1 = selected[0];
        auto* item2 = selected[1];

        if (item1 && item2) {
            auto seq1 = item1->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
            auto seq2 = item2->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();

            addToList( seq1->concat( *seq2 ) );
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_skipBtn_clicked() 
{
    try {
        auto* item = ui->seqList->item( ui->seqList->currentRow());
        if (item) {
            auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
            if (!ui->index1Input->text().isEmpty() && !ui->index1Input->text().isEmpty()) {
                addToList(
                    seq->skip( getFromInput<Ordinal>(ui->index1Input->text())
                             , getFromInput<Ordinal>(ui->index2Input->text()))
                          );
            } else if (!ui->index1Input->text().isEmpty()) {
                addToList(
                    seq->skip( getFromInput<Ordinal>(ui->index1Input->text()))
                          );
            } else {
                throw Exception( Exception::ErrorCode::INVALID_INPUT );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_subSeqBtn_clicked() 
{
    try {
        auto* item = ui->seqList->item( ui->seqList->currentRow());
        if (item) {
            auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
            if (!ui->index1Input->text().isEmpty() && !ui->index1Input->text().isEmpty()) {
                addToList(
        seq->getSubSequence( getFromInput<Ordinal>(ui->index1Input->text())
                           , getFromInput<Ordinal>(ui->index2Input->text()))
                          );
            } else {
                throw Exception( Exception::ErrorCode::INVALID_INPUT );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_mapBtn_clicked() 
{
    try {
        auto* item = ui->seqList->item( ui->seqList->currentRow());
        if (item) {
            auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
            if (ui->mapFunc->currentIndex() == 0) {
                addToList( seq->map<int>( MainWindow::mapDouble() ));
            } else if (ui->mapFunc->currentIndex() == 1) {
                addToList( seq->map<int>( MainWindow::mapSign() ));
            } else {
                throw Exception( Exception::ErrorCode::INVALID_INPUT );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}

void MainWindow::on_whereBtn_clicked() 
{
    try {
        auto* item = ui->seqList->item( ui->seqList->currentRow());
        if (item) {
            auto seq = item->data(Qt::UserRole).value<SharedPtr<LazySequence<int>>>();
            if (ui->whereFunc->currentIndex() == 0) {
                addToList( seq->where( MainWindow::whereEven() ));
            } else {
                throw Exception( Exception::ErrorCode::INVALID_INPUT );
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_SELECTION );
        }
    } catch ( Exception& ex ) {
        showError( ex );
    }
}