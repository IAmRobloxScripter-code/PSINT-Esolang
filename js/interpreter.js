class PSINT_VM {
  constructor() {
    this.registers = {
      mommas: 0,
      dad: 0,
      speed: 0,
    };
    this.current_register = "mommas";
    this.output = "";
  }
}

const is_alpha = function (c) {
  return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_".includes(c);
};

const tokenize = function (source) {
  const tokens = [];
  const characters = source.split("");
  let position = 0;

  while (characters.length > 0) {
    const char = characters[0];

    if (char === "\n" || char === " " || char === "\t" || char === "\r") {
      characters.shift();
      position++;
      continue;
    }

    if (char === "#") {
      characters.shift();
      position++;
      while (
        characters.length > 0 &&
        characters[0] !== "\n" &&
        characters[0] !== "\r"
      ) {
        characters.shift();
        position++;
      }
      continue;
    }

    if (char === "{" || char === "}") {
      tokens.push([characters.shift(), position, position]);
      position++;
      continue;
    }

    if (is_alpha(char)) {
      let word = "";
      let start = position;
      while (characters.length > 0 && is_alpha(characters[0])) {
        word += characters.shift();
        position++;
      }
      tokens.push([word, start, position]);
      continue;
    }

    characters.shift();
    position++;
  }

  tokens.push(["eof", position, position]);
  return tokens;
};

const parse = function (tokens) {
  const token = tokens.shift();
  const value = token[0];
  const start = token[1];
  const end = token[2];

  if (value === "please") {
    const register = tokens.shift();
    return { kind: "set", value: register[0], start: start, end: register[2] };
  } else if (value === "need") {
    tokens.shift();
    const body = [];
    while (tokens[0][0] !== "eof" && tokens[0][0] !== "}") {
      body.push(parse(tokens));
    }
    let endv = tokens.shift()[2];
    return { kind: "loop", value: body, start: start, end: endv };
  } else if (value === "live") {
    return { kind: "add", start: start, end: end };
  } else if (value === "kinda") {
    return { kind: "sub", start: start, end: end };
  } else if (value === "homeless") {
    return { kind: "print", start: start, end: end };
  } else if (value === "my") {
    return { kind: "input", start: start, end: end };
  } else if (value === "with") {
    return { kind: "rotate_forward", start: start, end: end };
  } else if (value === "help") {
    return { kind: "rotate_backwards", start: start, end: end };
  } else {
    return { kind: "error", start: start, end: end };
  }
};

let running = false;
let steps = 0;
let step_per = 1;
let input = null;
let display = null;
let source = null;
let chunks_ran = 0;

function highlightRange(start, end) {
  const text = source.value;

  if (start < 0 || end > text.length || start >= end) return;

  const before = text.slice(0, start);
  const highlighted = text.slice(start, end);
  const after = text.slice(end);
  display.innerHTML = before + "<mark>" + highlighted + "</mark>" + after;
}

const run_instruction = async function (vm, ast) {
  steps++;
  const node = ast.shift();
  if (!node) return;

  highlightRange(node.start, node.end);

  if (node.kind === "set") {
    vm.current_register = node.value;
  } else if (node.kind === "add") {
    vm.registers[vm.current_register]++;
    if (vm.registers[vm.current_register] >= 256)
      vm.registers[vm.current_register] = 0;
    else if (vm.registers[vm.current_register] < 0)
      vm.registers[vm.current_register] = 255;
  } else if (node.kind === "sub") {
    vm.registers[vm.current_register]--;
    if (vm.registers[vm.current_register] >= 256)
      vm.registers[vm.current_register] = 0;
    else if (vm.registers[vm.current_register] < 0)
      vm.registers[vm.current_register] = 255;
  } else if (node.kind === "loop") {
    while (vm.registers[vm.current_register] !== 0 && running) {
      chunks_ran += 1
      if (chunks_ran > step_per) {
        chunks_ran = 0
        await new Promise((r) => setTimeout(r, 1000 / step_per));
      }
      const body = [...node.value];
      steps++;
      output.textContent =
        "Steps: " +
        steps +
        "\nMachine State -> Mommas: " +
        vm.registers.mommas +
        " | Dad: " +
        vm.registers.dad +
        " | Speed: " +
        vm.registers.speed +
        " | Current Register: " +
        vm.current_register +
        " | Output: " +
        vm.output;

      while (body.length > 0 && running) {
         chunks_ran += 1
          if (chunks_ran > step_per) {
            chunks_ran = 0
            await new Promise((r) => setTimeout(r, 1000 / step_per));
          }
        output.textContent =
          "Steps: " +
          steps +
          "\nMachine State -> Mommas: " +
          vm.registers.mommas +
          " | Dad: " +
          vm.registers.dad +
          " | Speed: " +
          vm.registers.speed +
          " | Current Register: " +
          vm.current_register +
          " | Output: " +
          vm.output;
        await run_instruction(vm, body);
      }
    }
  } else if (node.kind === "print") {
    if (vm.registers[vm.current_register] != 0) { 
      vm.output += String.fromCharCode(vm.registers[vm.current_register]);
    }
  } else if (node.kind === "rotate_forward") {
    const A = vm.registers.mommas;
    const B = vm.registers.dad;
    const C = vm.registers.speed;
    vm.registers.mommas = C;
    vm.registers.dad = A;
    vm.registers.speed = B;
  } else if (node.kind === "rotate_backwards") {
    const A = vm.registers.mommas;
    const B = vm.registers.dad;
    const C = vm.registers.speed;
    vm.registers.mommas = B;
    vm.registers.dad = C;
    vm.registers.speed = A;
  } else if (node.kind === "input") {
    if (input.value.length <= 0) { vm.registers[vm.current_register] = 0; return; };
    vm.registers[vm.current_register] = input.value[0].charCodeAt(0);
    input.value = input.value.slice(1);
  }
};

async function execute(output) {
  steps = 0;

  source.readOnly = true;
  input.readOnly = true;

  const tokens = tokenize(source.value);
  const ast = [];

  while (tokens.length > 0 && tokens[0] !== "eof" && running) {
    ast.push(parse(tokens));
  }

  const vm = new PSINT_VM();

  while (ast.length > 0 && running) {
    chunks_ran += 1
      if (chunks_ran > step_per) {
        chunks_ran = 0
        await new Promise((r) => setTimeout(r, 1000 / step_per));
      }
    output.textContent =
      "Steps: " +
      steps +
      "\nMachine State -> Mommas: " +
      vm.registers.mommas +
      " | Dad: " +
      vm.registers.dad +
      " | Speed: " +
      vm.registers.speed +
      " | Current Register: " +
      vm.current_register +
      " | Output: " +
      vm.output;
    await run_instruction(vm, ast);
  }

  source.readOnly = false;
  input.readOnly = false;
  output.textContent =
    vm.output +
    "\nSteps: " +
    steps +
    "\nMachine State -> Mommas: " +
    vm.registers.mommas +
    " | Dad: " +
    vm.registers.dad +
    " | Speed: " +
    vm.registers.speed +
    " | Current Register: " +
    vm.current_register +
    " | Output: " +
    vm.output;
  running = false;
}

function isValidNumber(str) {
  return !isNaN(str) && str.trim() !== "";
}

document.addEventListener("DOMContentLoaded", function () {
  const run = document.getElementById("run");
  const stop = document.getElementById("stop");
  const output = document.getElementById("output");
  const steps_input = document.getElementById("steps");
  source = document.getElementById("source-code");
  input = document.getElementById("input-code");
  display = document.getElementById("view-mode");

  if (!run || !stop || !source || !output || !input || !display) {
    alert("Could not load some DOM elements!");
    return;
  }

  run.addEventListener("click", function () {
    if (!running) {
      running = true;
      execute(output, source);
    }
  });

  stop.addEventListener("click", function () {
    output.textContent = "";
    running = false;
  });

  display.textContent = source.value;
  source.addEventListener("input", () => {
    if (running) {
      return;
    }
    display.textContent = source.value;
  });

  if (isValidNumber(steps_input.value)) {
    step_per = parseInt(steps_input.value);
  } else {
    step_per = 999;
    steps_input.value = 999;
  }

  steps_input.addEventListener("blur", () => {
    if (isValidNumber(steps_input.value)) {
      step_per = parseInt(steps_input.value);
    } else {
      step_per = 999;
      steps_input.value = 999;
    }
  });
});
