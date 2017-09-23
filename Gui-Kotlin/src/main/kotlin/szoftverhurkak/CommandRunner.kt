package szoftverhurkak

/**
 * Created by serle on 2017.09.23..
 */
object CommandRunner {
    fun runCommands(commands: List<Instruction>, senderProcedure: SenderProcedure) {
        Thread {
            commands.stream().forEach { element ->
                senderProcedure.sendMethod(element)
                println(element.toString())
                try {
                    Thread.sleep(element.time.toLong())
                } catch (e: InterruptedException) {
                    e.printStackTrace()
                }
            }
        }.start()
    }

}
