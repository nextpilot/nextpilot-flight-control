import typer


app = typer.Typer()


@app.command()
def update(force:bool=type.Option(False, help='Whether to force an upgrade')):

    

@app.command():
def upgrade():
    pass


@app.command()
def list():
    pass

if __name__ == "__main__":
    app()