#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

extern void backup_file(const std::string &src, const std::string &dst);

extern std::string root_path;

extern void restore_file(const std::string &path, const std::string &dst);

#include "encrypt.h"
#include "compress.h"

bool is_encrypt = false;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::on_toolButton_clicked() {
    QString filePath = QFileDialog::getExistingDirectory(this, "请选择备份目录路径…", "./");
    ui->src_path->setText(filePath);
}


void MainWindow::on_toolButton_2_clicked() {
    QString filePath = QFileDialog::getExistingDirectory(this, "请选择文件保存路径…", "./");
    ui->dst_path->setText(filePath);
}


void MainWindow::on_checkBox_stateChanged(int arg1) {
    is_encrypt = ui->checkBox->isChecked();
}


void MainWindow::on_backup_clicked() {
    if (ui->dst_path->text().isEmpty() || ui->src_path->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写完整信息");
        return;
    }
    root_path = ui->src_path->text().toStdString();
    QDir dir(ui->dst_path->text());
    backup_file(ui->src_path->text().toStdString(), ui->dst_path->text().toStdString() + "/backup.tmp");
//    QMessageBox::information(this, "compross", "compross");
    compress c;
    c.compress_file(ui->dst_path->text().toStdString() + "/backup.tmp", ui->dst_path->text().toStdString() + "/backup");
    dir.remove("backup.tmp");
    if (is_encrypt) {
        if (ui->password->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "警告", "请填写密码");
            return;
        }
        encrypt e(ui->password->toPlainText().toStdString());
        e.encrypt_file(ui->dst_path->text().toStdString() + "/backup",
                       ui->dst_path->text().toStdString() + "/backup.enc");
        dir.remove("backup");
    }
    QMessageBox::information(this, "备份完成", "备份完成");

}

void MainWindow::on_restore_clicked() {
    if (ui->dst_path->text().isEmpty() || ui->src_path->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写完整信息");
        return;
    }
    QDir dir(ui->dst_path->text());
    if (is_encrypt) {
        if (ui->password->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "警告", "请填写密码");
            return;
        }
        encrypt e(ui->password->toPlainText().toStdString());
        e.encrypt_file(ui->dst_path->text().toStdString() + "/backup.enc",
                       ui->dst_path->text().toStdString() + "/backup");
//        dir.remove("backup.enc");
    }
    compress c;
    c.decompress_file(ui->dst_path->text().toStdString() + "/backup",
                      ui->dst_path->text().toStdString() + "/backup.tmp");
//    dir.remove("backup");
    restore_file(ui->dst_path->text().toStdString() + "/backup.tmp", ui->src_path->text().toStdString());
    dir.remove("backup.tmp");
    QMessageBox::information(this, "恢复完成", "恢复完成");
}

