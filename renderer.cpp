
string Renderer::renderState(vector<bool> inst, vector<bool> adr, string output) {
  val sb = new StringBuilder()
  setPrinterOutput()
  val switchIndex: Map[Char, Int] = Map()

  for (line <- DRAWING.split('\n')) {
    val processedLine = insertActualValues(line, switchIndex)
    sb.append(processedLine + "\n")
}
sb.deleteCharAt(sb.size - 1)
sb.toString()A
}

def Renderer::setPrinterOutput() = {
  var outputLines = output.split("\n").reverse
  if (output.length <= 0) {
    printerOutput = "|0|______________|0|"
} else {
    outputLines = outputLines.map(line => "|0| " + line + " |0|")
    outputLines = outputLines :+ "|0|______________|0|"
    printerOutput = outputLines.mkString("")
}
}

def Renderer::insertActualValues(line: String, switchIndex: Map[Char, Int]): String = {
  val sb = new StringBuilder()
  for (c <- line) {
    val cOut = if ("[0-9a-z]".r.findAllIn(c.toString).length != 1) {
      c
  } else {
      getLightbulb(c, switchIndex)
  }
  sb.append(cOut)
}
sb.toString
}

def Renderer::getLightbulb(c: Char, switchIndex: Map[Char, Int]): Char = {
  val i = if (switchIndex.contains(c)) {
    val i = switchIndex.get(c).get
    switchIndex.put(c, i + 1)
    i
} else {
    switchIndex.put(c, 1)
    0
}

val patRam = "[0-9a-e]".r
c.toString() match {
    case "p" => Util.getChar(pcIsPointingToAddress(i))
    case "s" => Util.getChar(instructionIsPointingToAddress(i))
    case "r" => Util.getChar(reg(i))
    case "i" => Util.getChar(instructionHasId(i))
    case "o" => getFormattedOutput(i)
    case patRam => getRam(c, i)
}
}

def Renderer::pcIsPointingToAddress(addr: Int): Boolean = {
  Util.getInt(pc) == addr
}

def Renderer::instructionIsPointingToAddress(addr: Int): Boolean = {
  Util.getInt(Util.getSecondNibble(ram.get(pc))) == addr
}

def Renderer::instructionHasId(id: Int): Boolean = {
  Util.getInt(Util.getFirstNibble(ram.get(pc))) == id
}

def Renderer::getFormattedOutput(i: Int): Char = {
  if (printerOutput.length <= i)
    ' '
else
    printerOutput.charAt(i)
}

def Renderer::getRam(c: Char, i: Int): Char = {
  val j = Util.hexToInt(c)
  val ramLines = ram.getStr.split("\n")
  ramLines(j)(i)
}
}