package szoftverhurkak

import javafx.application.Application
import tornadofx.App

class TableViewApp : App() {
    override val primaryView = DroneTableView::class
}

fun main(args: Array<String>) {
    Application.launch(TableViewApp::class.java, *args)
}
