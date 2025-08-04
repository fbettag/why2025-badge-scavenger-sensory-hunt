# Contributing to WHY2025 Scavenger Sensory Hunt

Thank you for your interest in contributing to the WHY2025 badge project! This guide will help you get started.

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow
- Follow responsible disclosure for security issues

## How to Contribute

### Reporting Issues

1. Check existing issues first
2. Use issue templates when available
3. Include:
   - Clear description
   - Steps to reproduce
   - Expected vs actual behavior
   - Hardware/software versions

### Submitting Pull Requests

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/your-feature-name`
3. Make your changes
4. Test thoroughly on hardware if possible
5. Commit with clear messages
6. Push to your fork
7. Submit a PR with:
   - Description of changes
   - Related issue numbers
   - Test results
   - Screenshots if applicable

### Code Style

#### C/C++ Code
- Follow ESP-IDF coding conventions
- Use meaningful variable names
- Add comments for complex logic
- Keep functions focused and small
- Use header guards in .h files

#### Python Code
- Follow PEP 8
- Use type hints where appropriate
- Document functions with docstrings
- Run `black` for formatting

### Testing

Before submitting:

1. **Build Test**: Ensure project builds without warnings
   ```bash
   idf.py build
   ```

2. **Hardware Test**: If you have a badge, test on actual hardware

3. **Unit Tests**: Run any available unit tests

4. **Lint Check**: For Python code:
   ```bash
   flake8 tools/*.py
   ```

### Areas for Contribution

#### High Priority
- Actual sensor driver implementations (BME690, BMI270)
- Display driver for 720x720 LCD
- LoRa communication protocol
- TinyML model optimization

#### Medium Priority
- Additional quest ideas
- Improved data visualization
- Power optimization
- Documentation improvements

#### Good First Issues
- Fix typos in documentation
- Add more quest descriptions
- Improve error messages
- Add unit tests

### Development Setup

1. Clone the repository
2. Set up ESP-IDF v6.0
3. Install Python dependencies: `pip install -r requirements.txt`
4. Use VS Code with ESP-IDF extension (recommended)

### Commit Messages

Format: `type(scope): description`

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Formatting
- `refactor`: Code restructuring
- `test`: Tests
- `chore`: Maintenance

Example: `feat(sensors): add BME690 temperature compensation`

### Documentation

- Update README.md for user-facing changes
- Add inline comments for complex code
- Update ML_TRAINING_GUIDE.md for ML changes
- Include examples where helpful

### Security & Ethics

- Never commit credentials or keys
- Use smoke detection responsibly
- Follow local laws and regulations
- Consider privacy implications
- Report security issues privately

### Questions?

- Open a discussion issue
- Tag maintainers for guidance
- Check existing documentation
- Be patient - this is a volunteer project

## Recognition

Contributors will be acknowledged in:
- README.md contributors section
- Release notes
- WHY2025 conference materials (with permission)

Thank you for helping make the WHY2025 badge awesome! 🎉