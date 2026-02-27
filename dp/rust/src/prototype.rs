/// Prototype Pattern Implementation in Rust
///
/// The Prototype pattern creates objects by cloning existing instances.
/// In Rust, this is naturally implemented using the Clone trait.

use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// Prototype trait for cloneable objects
pub trait Prototype {
    fn clone_object(&self) -> Box<dyn Prototype>;
    fn get_info(&self) -> String;
}

/// Document types that can be cloned
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Resume {
    pub name: String,
    pub email: String,
    pub phone: String,
    pub experience_years: u32,
    pub skills: Vec<String>,
    pub education: Vec<String>,
    pub previous_jobs: Vec<String>,
}

impl Resume {
    pub fn new(name: &str, email: &str, phone: &str) -> Self {
        Self {
            name: name.to_string(),
            email: email.to_string(),
            phone: phone.to_string(),
            experience_years: 0,
            skills: Vec::new(),
            education: Vec::new(),
            previous_jobs: Vec::new(),
        }
    }

    pub fn add_skill(&mut self, skill: &str) {
        self.skills.push(skill.to_string());
    }

    pub fn add_education(&mut self, education: &str) {
        self.education.push(education.to_string());
    }

    pub fn add_job(&mut self, job: &str) {
        self.previous_jobs.push(job.to_string());
    }

    pub fn set_experience(&mut self, years: u32) {
        self.experience_years = years;
    }

    /// Deep clone with modifications
    pub fn clone_for_position(&self, position_skills: &[String]) -> Self {
        let mut cloned = self.clone();

        // Customize skills for specific position
        cloned.skills.extend(position_skills.iter().cloned());
        cloned.skills.sort();
        cloned.skills.dedup();

        cloned
    }
}

impl Prototype for Resume {
    fn clone_object(&self) -> Box<dyn Prototype> {
        Box::new(self.clone())
    }

    fn get_info(&self) -> String {
        format!(
            "Resume for {} - {} years experience, {} skills",
            self.name,
            self.experience_years,
            self.skills.len()
        )
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CoverLetter {
    pub recipient: String,
    pub company: String,
    pub position: String,
    pub content: String,
    pub signature: String,
}

impl CoverLetter {
    pub fn new(recipient: &str, company: &str, position: &str) -> Self {
        Self {
            recipient: recipient.to_string(),
            company: company.to_string(),
            position: position.to_string(),
            content: format!(
                "Dear {},\n\nI am writing to express my interest in the {} position at {}.",
                recipient, position, company
            ),
            signature: "Sincerely,\n[Your Name]".to_string(),
        }
    }

    pub fn customize_content(&mut self, custom_content: &str) {
        self.content = custom_content.to_string();
    }

    pub fn set_signature(&mut self, signature: &str) {
        self.signature = signature.to_string();
    }
}

impl Prototype for CoverLetter {
    fn clone_object(&self) -> Box<dyn Prototype> {
        Box::new(self.clone())
    }

    fn get_info(&self) -> String {
        format!(
            "Cover Letter for {} position at {}",
            self.position, self.company
        )
    }
}

/// Portfolio item for demonstrating different clone scenarios
#[derive(Debug, Clone)]
pub struct PortfolioProject {
    pub name: String,
    pub description: String,
    pub technologies: Vec<String>,
    pub repository_url: Option<String>,
    pub demo_url: Option<String>,
    pub screenshots: Vec<String>,
    pub completion_date: String,
}

impl PortfolioProject {
    pub fn new(name: &str, description: &str) -> Self {
        Self {
            name: name.to_string(),
            description: description.to_string(),
            technologies: Vec::new(),
            repository_url: None,
            demo_url: None,
            screenshots: Vec::new(),
            completion_date: "2023".to_string(),
        }
    }

    /// Shallow clone - shares references where possible
    pub fn shallow_clone(&self) -> Self {
        self.clone()
    }

    /// Deep clone with potential modifications
    pub fn deep_clone(&self) -> Self {
        let mut cloned = self.clone();

        // Could modify clone here if needed
        cloned.completion_date = "2023 (Cloned)".to_string();

        cloned
    }
}

impl Prototype for PortfolioProject {
    fn clone_object(&self) -> Box<dyn Prototype> {
        Box::new(self.clone())
    }

    fn get_info(&self) -> String {
        format!(
            "Project: {} ({} technologies)",
            self.name,
            self.technologies.len()
        )
    }
}

/// Prototype Registry for managing and cloning different document templates
pub struct DocumentRegistry {
    prototypes: HashMap<String, Box<dyn Prototype>>,
}

impl DocumentRegistry {
    pub fn new() -> Self {
        Self {
            prototypes: HashMap::new(),
        }
    }

    pub fn register(&mut self, key: &str, prototype: Box<dyn Prototype>) {
        self.prototypes.insert(key.to_string(), prototype);
    }

    pub fn clone_prototype(&self, key: &str) -> Option<Box<dyn Prototype>> {
        self.prototypes.get(key).map(|p| p.clone_object())
    }

    pub fn list_prototypes(&self) -> Vec<String> {
        self.prototypes.keys().cloned().collect()
    }

    pub fn get_prototype_info(&self, key: &str) -> Option<String> {
        self.prototypes.get(key).map(|p| p.get_info())
    }
}

impl Default for DocumentRegistry {
    fn default() -> Self {
        let mut registry = Self::new();

        // Register default templates
        let default_resume = Resume::new(
            "[Your Name]",
            "[your.email@example.com]",
            "[Your Phone]",
        );
        registry.register("basic_resume", Box::new(default_resume));

        let default_cover_letter = CoverLetter::new(
            "Hiring Manager",
            "[Company Name]",
            "[Position Title]",
        );
        registry.register("basic_cover_letter", Box::new(default_cover_letter));

        let default_project = PortfolioProject::new(
            "[Project Name]",
            "[Project Description]",
        );
        registry.register("basic_project", Box::new(default_project));

        registry
    }
}

/// Advanced cloning with configuration
#[derive(Debug, Clone)]
pub struct CloneConfiguration {
    pub include_personal_info: bool,
    pub include_contact_info: bool,
    pub include_experience: bool,
    pub customize_for_role: Option<String>,
}

impl Default for CloneConfiguration {
    fn default() -> Self {
        Self {
            include_personal_info: true,
            include_contact_info: true,
            include_experience: true,
            customize_for_role: None,
        }
    }
}

impl Resume {
    pub fn clone_with_config(&self, config: &CloneConfiguration) -> Self {
        let mut cloned = if config.include_personal_info {
            self.clone()
        } else {
            Resume::new("[Redacted]", "[Redacted]", "[Redacted]")
        };

        if !config.include_contact_info {
            cloned.email = "[Redacted]".to_string();
            cloned.phone = "[Redacted]".to_string();
        }

        if !config.include_experience {
            cloned.previous_jobs.clear();
            cloned.experience_years = 0;
        }

        if let Some(ref role) = config.customize_for_role {
            // Add role-specific skills
            match role.as_str() {
                "developer" => {
                    cloned.add_skill("Programming");
                    cloned.add_skill("Software Development");
                }
                "manager" => {
                    cloned.add_skill("Leadership");
                    cloned.add_skill("Project Management");
                }
                "designer" => {
                    cloned.add_skill("UI/UX Design");
                    cloned.add_skill("Creative Design");
                }
                _ => {}
            }
        }

        cloned
    }
}

/// Factory for creating different types of document prototypes
pub struct DocumentPrototypeFactory;

impl DocumentPrototypeFactory {
    pub fn create_tech_resume() -> Resume {
        let mut resume = Resume::new("Tech Professional", "tech@example.com", "123-456-7890");
        resume.set_experience(5);
        resume.add_skill("Rust");
        resume.add_skill("Python");
        resume.add_skill("JavaScript");
        resume.add_skill("Docker");
        resume.add_education("Computer Science Degree");
        resume.add_job("Software Engineer at TechCorp");
        resume.add_job("Junior Developer at StartupXYZ");
        resume
    }

    pub fn create_management_resume() -> Resume {
        let mut resume = Resume::new("Manager Professional", "manager@example.com", "123-456-7891");
        resume.set_experience(8);
        resume.add_skill("Leadership");
        resume.add_skill("Project Management");
        resume.add_skill("Team Building");
        resume.add_skill("Strategic Planning");
        resume.add_education("MBA");
        resume.add_education("Business Administration Degree");
        resume.add_job("Senior Manager at BigCorp");
        resume.add_job("Team Lead at MidCorp");
        resume
    }

    pub fn create_designer_resume() -> Resume {
        let mut resume = Resume::new("Creative Designer", "designer@example.com", "123-456-7892");
        resume.set_experience(4);
        resume.add_skill("Adobe Creative Suite");
        resume.add_skill("UI/UX Design");
        resume.add_skill("Figma");
        resume.add_skill("Sketch");
        resume.add_education("Design Degree");
        resume.add_job("UI Designer at DesignStudio");
        resume.add_job("Graphic Designer at Agency");
        resume
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_resume_clone() {
        let original = DocumentPrototypeFactory::create_tech_resume();
        let cloned = original.clone();

        assert_eq!(original.name, cloned.name);
        assert_eq!(original.skills.len(), cloned.skills.len());

        // Ensure they are separate instances
        assert_ne!(
            original.skills.as_ptr(),
            cloned.skills.as_ptr()
        );
    }

    #[test]
    fn test_registry_operations() {
        let mut registry = DocumentRegistry::new();
        let resume = DocumentPrototypeFactory::create_tech_resume();

        registry.register("tech_resume", Box::new(resume));

        let cloned = registry.clone_prototype("tech_resume");
        assert!(cloned.is_some());

        let prototypes = registry.list_prototypes();
        assert!(prototypes.contains(&"tech_resume".to_string()));
    }

    #[test]
    fn test_clone_with_configuration() {
        let original = DocumentPrototypeFactory::create_tech_resume();

        let config = CloneConfiguration {
            include_personal_info: false,
            include_contact_info: true,
            include_experience: true,
            customize_for_role: Some("manager".to_string()),
        };

        let cloned = original.clone_with_config(&config);
        assert_eq!(cloned.name, "[Redacted]");
        assert!(cloned.skills.contains(&"Leadership".to_string()));
    }

    #[test]
    fn test_cover_letter_clone() {
        let original = CoverLetter::new("John Doe", "TechCorp", "Developer");
        let cloned = original.clone();

        assert_eq!(original.company, cloned.company);
        assert_eq!(original.position, cloned.position);
    }

    #[test]
    fn test_portfolio_project_clone() {
        let mut original = PortfolioProject::new("Web App", "A cool web application");
        original.technologies.push("React".to_string());
        original.technologies.push("Node.js".to_string());

        let cloned = original.deep_clone();
        assert_eq!(original.technologies.len(), cloned.technologies.len());
        assert_ne!(original.completion_date, cloned.completion_date);
    }

    #[test]
    fn test_prototype_trait() {
        let resume = DocumentPrototypeFactory::create_tech_resume();
        let prototype: Box<dyn Prototype> = Box::new(resume);

        let cloned = prototype.clone_object();
        assert!(cloned.get_info().contains("Tech Professional"));
    }
}

/// Example usage and demonstration
pub fn demo() {
    println!("=== Prototype Pattern Demo ===");

    // Basic cloning demonstration
    println!("\n1. Basic Resume Cloning:");
    let original_resume = DocumentPrototypeFactory::create_tech_resume();
    println!("Original: {}", original_resume.get_info());

    let cloned_resume = original_resume.clone();
    println!("Cloned: {}", cloned_resume.get_info());

    // Registry usage
    println!("\n2. Prototype Registry:");
    let mut registry = DocumentRegistry::default();

    // Add custom prototypes
    registry.register("tech_resume", Box::new(DocumentPrototypeFactory::create_tech_resume()));
    registry.register("manager_resume", Box::new(DocumentPrototypeFactory::create_management_resume()));

    println!("Available prototypes: {:?}", registry.list_prototypes());

    if let Some(tech_resume) = registry.clone_prototype("tech_resume") {
        println!("Cloned from registry: {}", tech_resume.get_info());
    }

    // Advanced cloning with configuration
    println!("\n3. Configured Cloning:");
    let original = DocumentPrototypeFactory::create_tech_resume();

    let config = CloneConfiguration {
        include_personal_info: true,
        include_contact_info: false,
        include_experience: true,
        customize_for_role: Some("manager".to_string()),
    };

    let configured_clone = original.clone_with_config(&config);
    println!("Original skills: {:?}", original.skills);
    println!("Configured clone skills: {:?}", configured_clone.skills);
    println!("Email redacted: {}", configured_clone.email);

    // Position-specific cloning
    println!("\n4. Position-Specific Cloning:");
    let base_resume = DocumentPrototypeFactory::create_tech_resume();

    let backend_skills = vec!["Database Design".to_string(), "API Development".to_string()];
    let backend_resume = base_resume.clone_for_position(&backend_skills);

    let frontend_skills = vec!["React".to_string(), "CSS".to_string()];
    let frontend_resume = base_resume.clone_for_position(&frontend_skills);

    println!("Base resume skills: {:?}", base_resume.skills);
    println!("Backend resume skills: {:?}", backend_resume.skills);
    println!("Frontend resume skills: {:?}", frontend_resume.skills);

    // Cover letter cloning
    println!("\n5. Cover Letter Cloning:");
    let base_cover_letter = CoverLetter::new("Hiring Manager", "TechCorp", "Software Engineer");
    let mut customized_cover_letter = base_cover_letter.clone();

    customized_cover_letter.customize_content(
        "I am excited to apply for the Software Engineer position. My experience with Rust and system programming makes me an ideal candidate."
    );

    println!("Base: {}", base_cover_letter.get_info());
    println!("Customized: {}", customized_cover_letter.get_info());

    // Portfolio project cloning
    println!("\n6. Portfolio Project Cloning:");
    let mut base_project = PortfolioProject::new("E-commerce Platform", "Full-stack web application");
    base_project.technologies.push("Rust".to_string());
    base_project.technologies.push("React".to_string());
    base_project.technologies.push("PostgreSQL".to_string());

    let variation1 = base_project.shallow_clone();
    let variation2 = base_project.deep_clone();

    println!("Base: {}", base_project.get_info());
    println!("Shallow clone: {}", variation1.get_info());
    println!("Deep clone: {}", variation2.get_info());
    println!("Deep clone date: {}", variation2.completion_date);
}

fn main() {
    demo();
}