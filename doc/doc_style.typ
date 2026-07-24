#let doc_style(body) = {
  set page(
    margin: (x: 2cm, y: 2cm),
  )

  let primary_color = rgb("#0b5100")
  let secondary_color = rgb("#ffa87a")
  let code_background = rgb("#e8f4f8")
  let code_border = rgb("#000000")

  //set heading(numbering: "1.")
  
  set text(
    font: "NotoSerifTamilSlanted",
    size: 10pt
  )

  show heading.where(level: 1): it => block(
    fill: primary_color,
    stroke: primary_color,
    inset: (x: .5em, y: .2em),
    radius: .2em,
    text(fill: white)[#it.body]
  )

  show heading.where(level: 2): it => block(
    fill: secondary_color,
    stroke: secondary_color,
    inset: (x: .3em, y: .1em),
    radius: .1em,
    text(fill: black)[#it.body]
  )

  show heading.where(level: 3): it => {
    set text(weight: "bold", size: 12pt)
    it
  }

  show heading.where(level: 4): it => {
    set text(weight: "bold", size: 11pt)
    it
  }

  show raw.where(block: false): set text(
    font: "Hack",
    size: 9pt,
    fill: rgb("#1a00ad"),
  )

  show raw.where(block: true): it => block(
    fill: code_background,
    stroke: (paint: code_border, thickness: 0.5pt),
    inset: 8pt,
    radius: 4pt,
    text(
      font: "Hack",
      size: 9pt,
    )[
      #it
    ]
  )

  show list: it => {
    set text(fill: black)
    it
  }

  body
}